#include "stdafx.h"
#include "World.h"
#include "MessagePerformAction.h"
#include "MessageSetWorld.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentBrainKamikaze.h"

ComponentBrainKamikaze::ComponentBrainKamikaze(BlackBoardUID uid,
											   BlackBoard *blackBoard)
: ComponentBrain(uid, blackBoard)
{
	registerMessageHandler("MessagePhysicsCollision", bind(&ComponentBrainKamikaze::handleMessagePhysicsCollision, this, _1));
}

void ComponentBrainKamikaze::resetMembers()
{
	ComponentBrain::resetMembers();
	nextState = StateWander;
	state = StateWander;
	targetPlayer = 0;
	resumeWanderThreshold = 5000.0f;
	maxSightDistance = 7.0f;
	wanderTimer = 0.0f;
	wanderAngle = 0.0f;
}

void ComponentBrainKamikaze::load(const PropertyBag &data)
{
	resetMembers();
	data.get("maxSightDistance", maxSightDistance); // optional tag
}

void ComponentBrainKamikaze::draw() const
{
	ComponentBrain::draw();

	if(displayDebugData)
	{
		CHECK_GL_ERROR();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glPopAttrib();
		CHECK_GL_ERROR();
	}
}

bool ComponentBrainKamikaze::canDetectAnyPlayer()
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

bool ComponentBrainKamikaze::canDetectPlayer(const Actor &player)
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
		// Do a fine-detail check against physics geometry (e.g. walls)
		return rayCast(player,
		               ourPosition3D,
		               playerPosition3D - ourPosition3D,
		               maxSightDistance);
	}

	return false;
}

Action ComponentBrainKamikaze::getNextAction(float milliseconds)
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

Action ComponentBrainKamikaze::getNextAction_wander(float milliseconds)
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

Action ComponentBrainKamikaze::getNextAction_chase(float milliseconds)
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

	return getActionFromAngle(angle);
}

void ComponentBrainKamikaze::handleMessagePhysicsCollision(Message &message)
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
