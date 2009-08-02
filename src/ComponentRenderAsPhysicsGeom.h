#ifndef COMPONENT_RENDER_AS_PHYSICS_GEOM_H
#define COMPONENT_RENDER_AS_PHYSICS_GEOM_H

#include "PropertyBag.h"
#include "AnimationController.h"
#include "Component.h"

/**
Renders physics geometry of the object to represent the object in the game.
Requires the presence of a ComponentPhysics in the object!
*/
class ComponentRenderAsPhysicsGeom : public Component
{
public:
	virtual string getTypeString() const { return "RenderAsPhysicsGeom"; }

	ComponentRenderAsPhysicsGeom(BlackBoardUID _uid,
	                             BlackBoard *_blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Updates the object */
	virtual void update(float milliseconds);
};

#endif
