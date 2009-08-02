#ifndef COMPONENT_EXPLODE_ON_DEATH_H
#define COMPONENT_EXPLODE_ON_DEATH_H

#include "PropertyBag.h"
#include "Component.h"

class World;

/**
When the actor dies (assuming it can die), create an explosion effect
with splash damage and
*/
class ComponentExplodeOnDeath : public Component
{
public:
	virtual string getTypeString() const { return "ExplodeOnDeath"; }

	ComponentExplodeOnDeath(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds) { /* Do Nothing */ }

private:
	void handleMessageCharacterHasDied(Message &message);
	void handleMessageOrientationHasBeenSet(Message &message);
	void handleMessagePositionHasBeenSet(Message &message);
	void handleMessageSetWorld(Message &message);

private:
	vec3 lastReportedPosition;
	mat3 lastReportedOrientation;
	World *world;

	int baseDamage;
	FileName soundFileName;
	FileName particlesFileName;
};

#endif
