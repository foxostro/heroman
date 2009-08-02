#ifndef COMPONENT_BRAIN_H
#define COMPONENT_BRAIN_H

#include "PropertyBag.h"
#include "Component.h"
#include "Actions.h"

/** Brain component implements some behavior for the actor */
class ComponentBrain : public Component
{
protected:
	int currentWaypoint;
	vector<vec2> waypoints;

private:
	float timeInState;
	vec3 lastReportedPosition;
	Action previousAction;
	World *world;
	Direction currentFacing;
	float waypointDistanceThreshold;

public:
	virtual string getTypeString() const { return "Brain"; }

	ComponentBrain(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data) = 0;

	/**
	Updates component each tick
	@param milliseconds Milliseconds since the last update
	*/
	virtual void update(float milliseconds);

	/** Draws any visual component represented here */
	virtual void draw() const;

	/** Resets all object members to defaults */
	virtual void resetMembers();

protected:
	/**
	Sub-classes of the brain component can specify a function to get the
	next action for the actor.
	@param milliseconds Milliseconds since the last update
	*/
	virtual Action getNextAction(float milliseconds) = 0;

	virtual void drawVector(vec2 p) const;
	virtual void drawPreviousDirectionVector() const;
	virtual void drawWaypointPath() const;
	virtual void drawWaypointBlips() const;
	virtual void drawVectorToDesiredWaypoint() const;

	Direction getCurrentFacing() const
	{
		return currentFacing;
	}

	inline vec2 getPosition2D() const
	{
		return vec2(lastReportedPosition.x, lastReportedPosition.y);
	}

	const vec3& getPosition3D() const
	{
		return lastReportedPosition;
	}

	float getTimeInState() const
	{
		return timeInState;
	}

	void resetTimeInState()
	{
		timeInState = 0.0f;
	}

	Action getPreviousAction() const
	{
		return previousAction;
	}

	World& getWorld() const
	{
		ASSERT(world, "Member \"world\" was null");
		return *world;
	}

	/** Loads a waypoint list from data */
	virtual void loadWaypointList(const PropertyBag &data);

	/** Increments the waypoint counter if we are close enough to the wp */
	virtual void maybeIncWaypointCounter();

	/** Returns the index of the nearest waypoint */
	virtual int findNearestWaypoint() const;

	/**
	Gets the distance between two vectors
	@param a a 2D vector that is different from b
	@param b a 2D vector that is different from a
	@return distance
	*/
	static float getDistance(const vec2 &a, const vec2 &b)
	{
		return sqrtf(SQR(a.x-b.x) + SQR(a.y-b.y));
	}
	
	/** Determines whether a given actor is dead */
	static bool isDead(const ActorPtr actor)
	{
		return actor == 0; // STUB
	}

	/** Get an actors' physics geom, or 0 */
	static dGeomID getGeom(const Actor &actor);

	/** Get an actors' physics geom, or 0 */
	static dGeomID getGeom(ActorPtr actor);

	/**
	@param actor Actor to test the ray against
	@param p Origin of the ray in space
	@param d Direction of the ray
	@param l Ray terminates after a given length
	*/
	bool rayCast(const Actor &actor,
	             const vec3 &p,
				 const vec3 &d,
				 float l);

private:
	void handleMessagePosition(Message &message);
	void handleMessageSetWorld(Message &message);
};

#endif
