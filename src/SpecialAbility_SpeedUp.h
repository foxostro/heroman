#ifndef SPECIAL_ABILITY_SPEED_UP_H
#define SPECIAL_ABILITY_SPEED_UP_H

#include "PropertyBag.h"
#include "Actor.h"
#include "SpecialAbility.h"
#include "ComponentSpecialAbility.h"

/**
Ability: Speed Up
This ability speeds up the player for a certain time period. Therefore, it should
obviously only be used with characters that move.
*/
class SpecialAbility_SpeedUp : SpecialAbility
{
public:

	/** Constructor */
	SpecialAbility_SpeedUp(ActorPtr a, string n, World *w);

	/** Desctructor */
	~SpecialAbility_SpeedUp();

	/** 
	Performs the functions associated with this ability.
	@return whether the ability was successfully executed
	*/
	virtual bool executeAbility();

	/**
	Displays any visual effects or information associated with this component.
	*/
	virtual void display();

	/** 
	Loads data necessary to construct the ability.
	@param data the data to load
	*/
	virtual void onLoad(const PropertyBag &data);

	/**
	Updates the ability, if necessary.
	@param milliseconds the time (in milliseconds) since the last update
	*/
	virtual void update(float milliseconds);

protected:
	/* -- Helper Functions -- */
	void chargeByDamageReceived(float milliseconds);	/** updates charge meter based on amount of time passed */

private:
	/* -- Instance Variables -- */
	unsigned int duration;		/** The amount of time that the effect lasts (in milliseconds) */
	float speedFactor;			/** The factor by which to multiply the character's speed */ 

	float originalSpeed;		/** Holds the original speed of the character */
	float originalCool;			/** holds the original cooldown time for the weapon */
	int timeLeft;				/** countdown until the effect is inactive again */

	int lastHealth;				/** the last health of the character */

	float timePassed;			/** the time passed, in milliseconds, since the last use of the ability */
};

#endif
