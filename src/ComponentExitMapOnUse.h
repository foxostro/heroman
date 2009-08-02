#ifndef COMPONENT_EXIT_MAP_ON_USE_H
#define COMPONENT_EXIT_MAP_ON_USE_H

#include "PropertyBag.h"
#include "World.h"
#include "Component.h"

/**
The trigger can be fired by an actor that is close enough to the trigger
object.  Then, the trigger will caus the game to exit the map and move to
the next map in the map list.
*/
class ComponentExitMapOnUse : public Component
{
public:
	virtual string getTypeString() const { return "ExitMapOnUse"; }

	ComponentExitMapOnUse(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds);

private:
	void handleMessageSetWorld(Message &message);
	void handleMessageClientUsesObject(Message &message);

	/**
	Determines whether or not the coin requirements to use the exit game
	have been satisied in the map.
	*/
	bool coinRequirementSatisfied() const;

private:
	/** Exit gate will lead to this map */
	FileName nextMap;

	/** Indicates that the gate requires some coins to be collected */
	bool requiresCoins;

	/** Game world */
	World *world;

	/** Sound to play if the exit gates fails to activate when used */
	FileName sfxOnFail;
};

#endif
