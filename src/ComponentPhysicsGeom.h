#ifndef COMPONENT_PHYSICS_GEOM_H
#define COMPONENT_PHYSICS_GEOM_H

#include "PropertyBag.h"
#include "AnimationController.h"
#include "ComponentPhysics.h"

class PhysicsEngine;

/**
Object is a piece of physics engine geometry, but is not itself a rigid body.
Object cannot be affected physically by objects in the physics engine, but can
affect physics engine objects through when they collide with this object.
*/
class ComponentPhysicsGeom : public ComponentPhysics
{
public:
	ComponentPhysicsGeom(BlackBoardUID _uid, BlackBoard *_blackBoard);

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Declares the position and orientation for the frame */
	virtual void update(float milliseconds);

	/** Draws the object */
	virtual void draw() const;

	/** Resets all object members to defaults */
	virtual void resetMembers();

	virtual vec3 getPosition() const;
	virtual void setPosition(const vec3 &position);

	virtual mat3 getOrientation() const;
	virtual void setOrientation(const mat3 &orientation);

	virtual dGeomID getGeom() const
	{
		return geom;
	}

	virtual dBodyID getBody() const
	{
		return 0;
	}

	virtual float getRadius() const
	{
		return collisionRadius;
	}

private:
	/** Called when the actor is about to be removed from the game world */
	void handleMessageZombifyActor(Message &_message);

	void handleMessageSetWorld(Message &m);
	void handleMessageRequestPositionBeSet(Message &m);
	void handleMessageRequestLookAt(Message &m);
	void handleMessageRequestSetOrientation(Message &m);
	void handleMessageDeclareInitialPosition(Message &m);

	void createGeom(const string &type);
	void createGeomBox();
	void createGeomCylinder();
	void createGeomCapsule();
	void createGeomSphere();

	/** Draw local axes for this object */
	void drawAxes() const;

	/** Draw the physics engine geometry for this object */
	void drawPhysicsGeom() const;

private:
	dGeomID geom;
	shared_ptr<PhysicsEngine> physicsEngine;

	float collisionRadius;
	float desiredHeight;
};

#endif
