#ifndef WEAPON_GRENADE_LAUNCHER_H
#define WEAPON_GRENADE_LAUNCHER_H

#include "PropertyBag.h"
#include "World.h"
#include "WeaponItem.h"

/** Grenade lsuncher weapon */
class WeaponItem_GrenadeLauncher : public WeaponItem
{
public:
	/**
	Constructor
	@param world Game world
	@param actor Actor that owns this weapon
	*/
	WeaponItem_GrenadeLauncher(World *world, ActorPtr actor);

protected:
	/** Pulls the trigger to fire the weapon */
	virtual void onFire();

	/**
	Updates the weapon every tick that it has been ready
	@return state to transition to for the next tick
	*/
	virtual STATE onUpdateReady(float milliseconds) { return READY; }

	/**
	Updates the weapon every tick that it has been charging
	@return state to transition to for the next tick
	*/
	virtual STATE onUpdateCharge(float milliseconds) { return CHARGING; }

	/**
	Updates the weapon every tick that it has been cooling down
	@return state to transition to for the next tick
	*/
	virtual STATE onUpdateCooldown(float milliseconds) { return COOLDOWN; }

	/** Loads component data from the pool of all object data */
	virtual void onLoad(const PropertyBag &data);

private:
	float incline;
};

#endif
