#ifndef COMPONENT_DROPS_LOOT_H
#define COMPONENT_DROPS_LOOT_H

#include "PropertyBag.h"
#include "Component.h"

class World; // opaque

/** Contains actor settings pertaining to character nehavior on death */
class ComponentDropsLoot : public Component
{
public:
	virtual string getTypeString() const { return "DropsLoot"; }

	ComponentDropsLoot(BlackBoardUID uid, BlackBoard *blackBoard);

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
	void handleMessageCharacterHasBeenResurrected(Message &message);
	void handleMessageCharacterHasDied(Message &message);
	void handleMessageSetWorld(Message &message);

	/**
	Spawns a piece of loot dropped by the character.
	Chooses position and velocity automatically.
	@param lootFilename Filename of the loot's actor definition
	*/
	void spawnLoot(const FileName &lootFilename);

	/**
	Spawns a piece of loot dropped by the character
	@param lootFilename Filename of the loot's actor definition
	@param position Position of the loot as soon as spawned
	@param velocity Velocity of the loot as soon as spawned
	*/
	void spawnLoot(const FileName &lootFilename,
	               const vec3 &position,
				   const vec3 &velocity);

private:
	World *world;
	int maxLoots;
	ListBag<FileName> lootFileNames;
};

#endif
