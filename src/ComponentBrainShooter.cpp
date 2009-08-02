#include "stdafx.h"
#include "World.h"
#include "MessagePerformAction.h"
#include "MessageSetWorld.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentBrainShooter.h"

ComponentBrainShooter::ComponentBrainShooter(BlackBoardUID uid,
						                           BlackBoard *blackBoard)
: ComponentBrain(uid, blackBoard)
{
	registerMessageHandler("MessagePhysicsCollision", bind(&ComponentBrainShooter::handleMessagePhysicsCollision, this, _1));
}

void ComponentBrainShooter::resetMembers()
{
	ComponentBrain::resetMembers();
	nextState = StateWander;
	state = StateWander;
	targetPlayer = 0;
	fov = 45.0f;
	resumeWanderThreshold = 5000.0f;
	maxSightDistance = 7.0f;
	shootDistance = 5.0f;
	wanderTimer = 0.0f;
	wanderAngle = 0.0f;
}

void ComponentBrainShooter::load(const PropertyBag &data)
{
	resetMembers();

	data.get("fov", fov); // optional tag
	data.get("maxSightDistance", maxSightDistance); // optional tag
	data.get("shootDistance", shootDistance); // optional tag

	wanderAngle = FRAND_RANGE(0.0f, 2.0f * (float)M_PI);
}

void ComponentBrainShooter::draw() const
{
	ComponentBrain::draw();

	if(displayDebugData)
	{
		CHECK_GL_ERROR();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);		
		drawFOV();
		glPopAttrib();
		CHECK_GL_ERROR();
	}
}

bool ComponentBrainShooter::isAngleWithinFOV(float angle, 
                                                float minFOV,
                                                float maxFOV)
{
	if(minFOV < maxFOV)
	{
		if((angle >= minFOV) && (angle <= maxFOV))
		{
			return true;
		}
	}
	else // if(maxFOV < minFOV)
	{
		if((angle <= maxFOV) || (angle >= minFOV))
		{
			return true;
		}
	}

	return false;
}

bool ComponentBrainShooter::canDetectAnyPlayer()
{
	const vector<ActorPtr> &players = getWorld().players;

	for(vector<ActorPtr>::const_iterator i = players.begin();
		i != players.end(); ++i)
	{
		const ActorPtr playerPtr = *i;
		const Actor &player = *playerPtr;

		if(canDetectPlayer(player))
		{
			targetPlayer = playerPtr;
			return true;
		}
	}

	return false;
}

bool ComponentBrainShooter::canDetectPlayer(const Actor &player)
{
	ComponentPhysics* component =
		dynamic_cast<ComponentPhysics*>(player.getComponent("Physics"));

	if(!component) return false;

	vec3 playerPosition3D = component->getPosition();
	vec2 playerPosition = vec2(playerPosition3D.x, playerPosition3D.y);
	vec3 ourPosition3D = getPosition3D();
	vec2 ourPosition = getPosition2D();
	
	float distance = getDistance(playerPosition, ourPosition);
	
	if(distance < maxSightDistance)
	{
		float radian_fov = fov * float(M_PI / 180.0);
		float facingAngle = getAngleFromDirection(getCurrentFacing());
		float minFOV = angle_clamp(facingAngle - radian_fov);
		float maxFOV = angle_clamp(facingAngle + radian_fov);
	
		float angleToTarget = getAngle(playerPosition, ourPosition);
	
#if 0
		if(isAngleWithinFOV(angleToTarget, minFOV, maxFOV))
		{
			// Do a fine-detail check against physics geometry (e.g. walls)
			return rayCast(player,
			               ourPosition3D,
						   playerPosition3D - ourPosition3D,
						   maxSightDistance);
		}
#else
		// Do a fine-detail check against physics geometry (e.g. walls)
		return rayCast(player,
			           ourPosition3D,
		               playerPosition3D - ourPosition3D,
		               maxSightDistance);
#endif
	}

	return false;
}

Action ComponentBrainShooter::getNextAction(float milliseconds)
{
	Action action = InvalidAction;

	switch(state)
	{
	case StateWander: action = getNextAction_wander(milliseconds); break;
	case StateChase:  action = getNextAction_chase(milliseconds);  break;
	}

	if(state != nextState)
	{
		state = nextState;
		resetTimeInState();
	}

	return action;
}

Action ComponentBrainShooter::getNextAction_wander(float milliseconds)
{
	if(canDetectAnyPlayer())
	{
		nextState = StateChase;
	}

	if((wanderTimer -= milliseconds) <= 0.0f)
	{
		wanderTimer = FRAND_RANGE(500.0f, 2000.0f);

		float d = ((float)M_PI / 2.0f) * FRAND_RANGE(-1.0f, 1.0f);
		wanderAngle = getAngleFromDirection(getCurrentFacing()) + d;
		wanderAngle = angle_clamp(wanderAngle);
	}

	return getActionFromAngle(wanderAngle);
}

Action ComponentBrainShooter::getNextAction_chase(float milliseconds)
{
	if(!targetPlayer ||
		isDead(targetPlayer) ||
		(!canDetectPlayer(*targetPlayer) &&
		  getTimeInState()>resumeWanderThreshold))
	{
		nextState = StateWander;
	}
	
	ComponentPhysics* component =
	  dynamic_cast<ComponentPhysics*>(targetPlayer->getComponent("Physics"));
	
	if(!component)
		return Stand;

	vec3 playerPosition3D = component->getPosition();
	vec3 ourPosition3D = getPosition3D();

	vec2 playerPosition = playerPosition3D.xy();
	vec2 ourPosition = ourPosition3D.xy();

	float angle = getAngle(playerPosition, ourPosition);
	Direction desiredDirection = getDirectionFromAngle(angle);
	Direction currentFacing = getCurrentFacing();

	if(currentFacing == desiredDirection &&
	   getDistance(playerPosition, ourPosition) < shootDistance &&
	   rayCast(*targetPlayer,
	           ourPosition3D,
	           playerPosition3D - ourPosition3D,
	           maxSightDistance))
	{
		getParentBlackBoard().relayMessage(MessagePerformAction(Stand));
		return AttackOnce;
	}
	else
	{
		Action action = getActionFromAngle(angle);
		return action;
	}
}

void ComponentBrainShooter::drawFOV() const
{
	float radius = maxSightDistance;
	float radian_fov = fov * float(M_PI / 180.0);
	float facingAngle = getAngleFromDirection(getCurrentFacing());
	float minFOV = angle_clamp(facingAngle - radian_fov);
	float maxFOV = angle_clamp(facingAngle + radian_fov);

	vec2 p = getPosition2D();
	vec2 a = vec2(cosf(minFOV), sinf(minFOV))*(radius+0.25f);
	vec2 b = vec2(cosf(maxFOV), sinf(maxFOV))*(radius+0.25f);
	vec2 c = vec2(cosf(facingAngle), sinf(facingAngle))*(radius+0.25f);

	glLineWidth(1.0f);

	glBegin(GL_LINES);
		glColor4fv(yellow);
		glVertex3f(p.x, p.y, 0.5f);
		glVertex3f(p.x + a.x, p.y + a.y, 0.5f);

		glColor4fv(yellow);
		glVertex3f(p.x, p.y, 0.5f);
		glVertex3f(p.x + b.x, p.y + b.y, 0.5f);

		glColor4fv(green);
		glVertex3f(p.x, p.y, 0.5f);
		glVertex3f(p.x + c.x, p.y + c.y, 0.5f);
	glEnd();

	glColor4fv(state==StateChase ? red : yellow);
	glBegin(GL_LINE_LOOP);
	for(size_t j=0; j<16; ++j)
	{
		const float angle = (float)j / 8.0f * float(M_PI * 2.0);
		glVertex3f(radius * cosf(angle) + p.x,
		           radius * sinf(angle) + p.y,
		           0.5f);
	}
	glEnd();
}

void ComponentBrainShooter::handleMessagePhysicsCollision(Message &message)
{
	dGeomID o2 = message.getField<dGeomID>("o2");
	Actor *a2 = reinterpret_cast<Actor*>(dGeomGetData(o2));

	if(a2)
	{
		dGeomID o1 = message.getField<dGeomID>("o1");
		Actor *a1 = reinterpret_cast<Actor*>(dGeomGetData(o1));

		OBJECT_ID uid = getActorPtr()->getUID();

		if(a1 && a1->getUID() == uid)
		{
			wanderTimer = 0.0f;
		}
	}
}
