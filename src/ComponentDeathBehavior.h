#ifndef COMPONENT_DEATH_BEHAVIOR_H
#define COMPONENT_DEATH_BEHAVIOR_H

#include "PropertyBag.h"
#include "Component.h"
#include "DeathBehavior.h"

/** Contains actor settings pertaining to character nehavior on death */
class ComponentDeathBehavior : public Component
{
public:
	virtual string getTypeString() const { return "DeathBehavior"; }

	ComponentDeathBehavior(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds) { /* Do Nothing */ }

	/** Gets character death behavior */
	DeathBehavior getDeathBehavior() const
	{
		return deathBehavior;
	}

private:
	void handleMessageCharacterHasBeenResurrected(Message &message);
	void handleMessageCharacterHasDied(Message &message);

	static DeathBehavior deathBehaviorFromString(const string &_s);

private:
	DeathBehavior deathBehavior;
};

#endif
