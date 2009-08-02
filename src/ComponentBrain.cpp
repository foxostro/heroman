#include "stdafx.h"
#include "World.h"
#include "MessagePerformAction.h"
#include "MessageSetWorld.h"
#include "MessagePlaySound.h"
#include "ComponentBrain.h"
#include "ComponentPhysics.h"

ComponentBrain::ComponentBrain(BlackBoardUID uid,
						       BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessagePositionHasBeenSet", bind(&ComponentBrain::handleMessagePosition, this, _1));
	registerMessageHandler("MessageSetWorld",           bind(&ComponentBrain::handleMessageSetWorld, this, _1));
}

void ComponentBrain::resetMembers()
{
	lastReportedPosition.zero();
	currentWaypoint = 0;
	waypoints.clear();
	previousAction = InvalidAction;
	timeInState = 0.0f;
	currentFacing = SouthWest;
	waypointDistanceThreshold = 0.25f;
}

void ComponentBrain::update(float milliseconds)
{
	timeInState += milliseconds;

	previousAction = getNextAction(milliseconds);

	switch(previousAction)
	{
	case StepEast:      currentFacing = East;      break;
	case StepNorthEast: currentFacing = NorthEast; break;
	case StepNorth:     currentFacing = North;     break;
	case StepNorthWest: currentFacing = NorthWest; break;
	case StepWest:      currentFacing = West;      break;
	case StepSouthWest: currentFacing = SouthWest; break;
	case StepSouth:     currentFacing = South;     break;
	case StepSouthEast: currentFacing = SouthEast; break;
	}

	getParentBlackBoard().relayMessage(MessagePerformAction(previousAction));
}

void ComponentBrain::handleMessagePosition(Message &message)
{
	lastReportedPosition = message.getField<vec3>("position");
}

void ComponentBrain::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
}

void ComponentBrain::draw() const
{
	if(displayDebugData)
	{
		CHECK_GL_ERROR();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		drawWaypointPath();
		drawWaypointBlips();
		drawVectorToDesiredWaypoint();
		drawPreviousDirectionVector();
		glPopAttrib();
		CHECK_GL_ERROR();
	}
}

void ComponentBrain::drawPreviousDirectionVector() const
{
	const vec2 x(-1,  1);
	const vec2 y(-1, -1);
	switch(previousAction)
	{
	case StepNorth:     drawVector(y);      break;
	case StepSouth:     drawVector(-y);     break;
	case StepEast:      drawVector(x);      break;
	case StepWest:      drawVector(-x);     break;
	case StepNorthEast: drawVector(y+x);    break;
	case StepNorthWest: drawVector(y-x);    break;
	case StepSouthEast: drawVector(-y+x);   break;
	case StepSouthWest: drawVector(-y-x);   break;
	default: break;
	}
}

void ComponentBrain::drawVector(vec2 p) const
{
	p = p.getNormal() + getPosition2D();

	glLineWidth(1.0f);
	glColor4fv(red);
	glBegin(GL_LINES);
	glVertex3f(lastReportedPosition.x, lastReportedPosition.y, 0.5f);
	glVertex3f(p.x, p.y, 0.5f);
	glEnd();
}

void ComponentBrain::drawVectorToDesiredWaypoint() const
{
	if(!waypoints.empty())
	{
		glLineWidth(1.0f);
		glColor4fv(red);
		glBegin(GL_LINES);
		glVertex3f(lastReportedPosition.x, lastReportedPosition.y, 0.5f);
		const vec2 wp = waypoints[currentWaypoint];
		glVertex3f(wp.x, wp.y, 0.5f);
		glEnd();
	}
}

void ComponentBrain::drawWaypointBlips() const
{
	CHECK_GL_ERROR();
	glLineWidth(2.0f);
	glColor4fv(yellow);
	for(vector<vec2>::const_iterator i=waypoints.begin();
		i!=waypoints.end(); ++i)
	{
		const float radius = 0.25f;
		glBegin(GL_LINE_LOOP);
		for(size_t j=0; j<8; ++j)
		{
			const float angle = (float)j / 8.0f * float(M_PI * 2.0);
			glVertex3f(radius * cosf(angle) + i->x,
				radius * sinf(angle) + i->y,
				0.5f);
		}
		glEnd();
	}
	CHECK_GL_ERROR();
}

void ComponentBrain::drawWaypointPath() const
{
	glLineWidth(1.0f);
	glColor4fv(green);
	glBegin(GL_LINE_LOOP);
	for(vector<vec2>::const_iterator i=waypoints.begin();
		i!=waypoints.end(); ++i)
	{
		glVertex3f(i->x, i->y, 0.5f);
	}
	glEnd();
}

void ComponentBrain::maybeIncWaypointCounter()
{
	vec2 pos = getPosition2D();
	vec2 wp = waypoints[currentWaypoint];

	if(getDistance(wp, pos) < waypointDistanceThreshold)
	{
		currentWaypoint++;
		if(currentWaypoint >= (int)waypoints.size())
			currentWaypoint = 0;
	}
}

int ComponentBrain::findNearestWaypoint() const
{
	int nearestWaypoint=0;
	float minDistance = 9999.0f;

	for(int counter=0; counter < (int)waypoints.size(); ++counter)
	{
		const vec2 wp = waypoints[counter];
		const float dx = wp.x - lastReportedPosition.x;
		const float dy = wp.y - lastReportedPosition.y;
		const float distance = sqrtf(SQR(dx) + SQR(dy));

		if(distance < minDistance)
		{
			minDistance = distance;
			nearestWaypoint = counter;
		}
	}

	return nearestWaypoint;
}

void ComponentBrain::loadWaypointList( const PropertyBag &wpData )
{
	waypoints.clear();
	currentWaypoint=0;
	for(size_t i=0, n=wpData.getNumInstances("waypoint"); i<n; ++i)
	{

		const vec2 waypoint = wpData.getVec2("waypoint", i);
		waypoints.push_back(waypoint);
	}
}

dGeomID ComponentBrain::getGeom(ActorPtr actor)
{
	return getGeom(*actor);
}

dGeomID ComponentBrain::getGeom(const Actor &actor)
{
	ComponentPhysics* component =
		dynamic_cast<ComponentPhysics*>(actor.getComponent("Physics"));

	if(!component)
		return 0;
	else
		return component->getGeom();
}

bool ComponentBrain::rayCast(const Actor &actor,
							 const vec3 &p,
							 const vec3 &d,
							 float l)
{
	ASSERT(world!=0, "Member \"world\" world was null");
	return world->getPhysicsEngine()->rayCast(getGeom(actor),
	                                          getGeom(getActorPtr()),
	                                          p, d, l);
}
