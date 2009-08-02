#ifndef COMPONENT_SCORABLE_H
#define COMPONENT_SCORABLE_H

#include "PropertyBag.h"
#include "Component.h"
#include "World.h"
#include "Actor.h"

/** 
A list of all the ways in which this actor might influence the score. 
*/
enum ScoreBehavior
{
	OnDeath,		/** score changes when this actor dies */
	OnPickup,		/** score changes when this actor is picked up */
	OnDamage		/** score changes when this actor is damaged */
};

/**
This component describes what behavior of or effects on its actor
change the player's score in-game.
*/
class ComponentScorable : public Component
{
public:
	/* -- Functions -- */
	/** Constructor */
	ComponentScorable(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Destructor */
	~ComponentScorable(void);

	/** Returns type of component in string */
	virtual string getTypeString() const { return "Scorable"; }

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Updates the object */
	virtual void update(float milliseconds);

private:
	/* -- Functions -- */
	/** Message handlers */
	void handleMessageCharacterHasDied(Message &m);
	void handleMessageSetWorld(Message &m);
	void handleMessageClientUsesObject(Message &m);
	void handleMessageCharacterReceivesDamage(Message &m);

private:
	/* -- Instance Variables -- */
	float scoreMultiplier;				/** the multiplier of this actor, if applicable */
	map<ScoreBehavior, int> behaviors;	/** how the scoring is applied (can include multiple behaviors);
											the map links different behaviors and their associated
											score values */

	World *world;
};

#endif
