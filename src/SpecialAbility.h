#ifndef SPECIAL_ABILITY_H
#define SPECIAL_ABILITY_H

#include "PropertyBag.h"
#include "World.h"
#include "Actor.h"

/**
A list of all the special abilities for characters in the game.
*/
enum AbilityType
{
	SpeedUp,
	SlowEnemies,
	ImmobilizeEnemies,
	InvertDamage,
	DoubleHealth,
	DoubleAttackPower,
	OneShotKill,
	PoisonAttack,
	Regen,
	Invincible,
	Explosion
};

/**
This class defines an interface for an ability that may be used by a character.
*/
class SpecialAbility
{
public:

	/** 
	Constructor 
	@param 
	*/
	SpecialAbility(ActorPtr a, string n, World *w);

	/** Desctructor */
	~SpecialAbility();

	/** 
	Performs the functions associated with this ability.
	@return whether the ability was successfully executed
	*/
	virtual bool executeAbility() = 0;

	/**
	Displays any visual effects or information associated with this component.
	*/
	virtual void display() = 0;

	/** 
	Loads data necessary to construct the ability.
	@param data the data to load
	*/
	void load(const PropertyBag &data);

	/*
	Called on load to load subclass data.
	@param data the data to load
	*/
	virtual void onLoad(const PropertyBag &data) = 0;

	/**
	Returns whether this ability is currently activated.
	@return the state (active/inactive) of this ability
	*/
	inline bool isActive()
	{
		return bIsActive;
	}

	/**
	Returns the actors that owns this special ability.
	@return a pointer to the actor that owns this ability
	*/
	inline ActorPtr getMyActor()
	{
		return myActor;
	}

	/**
	Returns the filled percentage for this ability's charge meter.
	@return the percentage of this ability's charge meter that is filled (in integer form, 0-100)
	*/
	inline int getChargePercentage()
	{
		//int x = (int)ceil(currentCharge / (chargeMeter * 1.0f));
		//Log(itos(x), "", FileName("debugzor.txt"), 1);
		return (int)currentCharge;//ceil(currentCharge);
	}

	/**
	Returns the name of this ability. Note: name should match the AbilityType of
	the ability.
	@return a string representing the name of the abiliity
	*/
	inline string getName()
	{
		return name;
	}

	/**
	Returns the AbilityType of this ability.
	@return the type of this ability
	*/
	inline AbilityType getType()
	{
		return type;
	}

	/**
	Updates the ability, if necessary.
	@param milliseconds the time (in milliseconds) since the last update
	*/
	virtual void update(float milliseconds) = 0;

protected:
	/* -- Helper Functions -- */
	/** 
	Calls appropriate functions to update charge meter. This function in turn
	calls the necessary "charge..." helpers below to charge the ability's meter.
	This function is not required to be called, but is intended as a helper. It
	should be called by the update() function, if used. The user can define as
	many of the helper functions as he desires, and each defined helper allows
	the ability to be updated in that manner, if specified in the ability's definition
	file. However, the user is free to not use any of these functions and to design
	his own charging routine.
	*/
	void chargeByType(float milliseconds);

	//functions to update the charge meter in a certain manner
	virtual void chargeByTimePassed(float milliseconds){}			/** updates charge meter based on time elapsed */
	virtual void chargeByDamageReceived(float milliseconds){}		/** updates charge meter based on damage received */
	virtual void chargeByScore(float milliseconds){}				/** updates charge meter based on score value attained */

	/* -- Instance Variables -- */
	string name;					/** the name of this ability */
	AbilityType type;				/** the type of this ability */

	unsigned int chargeMeter;		/** the amount of charge needed for the execution of this ability */
	float currentCharge;			/** the current amount of charge */
	unsigned int initialCharge;		/** the initial amount of charge */
	
	bool bIsActive;					/** whether this ability is currently being executed */
	bool bIsCharged;				/** whether the ability is ready to be used */
	ActorPtr myActor;				/** a pointer to the owner of this ability */
	World *world;					/** a pointer to the world */

	map<string, float> chargeTypes;	/** defines how the charge meter is updated */
};

#endif
