#ifndef WEAPONS_H
#define WEAPONS_H

#include "PropertyBag.h"
#include "World.h"

class ComponentPhysics;
class ComponentMovement;

/** Defines an interface for interacting with weapon objects */
class WeaponItem
{
protected:
	enum STATE
	{
		READY,
		CHARGING,
		COOLDOWN
	};

public:
	/**
	Constructor
	@param world Game world
	@param actor Actor that owns this weapon
	*/
	WeaponItem(World *world, ActorPtr actor);

	/** Begins charging the weapon in antipation of an attack */
	void beginChargeUp();

	/** Fires the weapon and ends any charging period */
	void endChargeUp();

	/** Loads component data from the pool of all object data */
	void load(const PropertyBag &data);

	/** Updates the weapon every tick */
	void update(float milliseconds);

	/* sets weapon cooldown time */
	inline void setCoolDown(float c)
	{
		coolDownTime = c;
	}

	/* gets weapon cooldown time */
	inline float getCoolDown()
	{
		return coolDownTime;
	}

protected:
	/** Pulls the trigger to fire the weapon */
	virtual void onFire() = 0;

	/**
	Updates the weapon every tick that it has been ready
	@return state to transition to for the next tick
	*/
	virtual STATE onUpdateReady(float milliseconds) = 0;

	/**
	Updates the weapon every tick that it has been charging
	@return state to transition to for the next tick
	*/
	virtual STATE onUpdateCharge(float milliseconds) = 0;

	/**
	Updates the weapon every tick that it has been cooling down
	@return state to transition to for the next tick
	*/
	virtual STATE onUpdateCooldown(float milliseconds) = 0;

	/** Loads component data from the pool of all object data */
	virtual void onLoad(const PropertyBag &data) = 0;

	/** Gets the game world */
	const World& getWorld() const
	{
		ASSERT(world, "Null member: world");
		return *world;
	}

	/** Gets the game world */
	World& getWorld()
	{
		ASSERT(world, "Null member: world");
		return *world;
	}

	/** Gets the actor that owns this weapon */
	Actor& getActor()
	{
		ASSERT(actor, "Null member: actor");
		return *actor;
	}

	/** Gets the actor that owns this weapon */
	const Actor& getActor() const
	{
		ASSERT(actor, "Null member: actor");
		return *actor;
	}

	/** Get time spent within the current state */
	float getTimeInState() const
	{
		return timeInState;
	}

	/**
	Plays a sound file
	@param fileName Sound file name
	*/
	void playSound(const FileName &fileName) const;

	/**
	Spawns a projectile object
	@param direction Direction to shoot the projectile in
	@param rigidBody Physics component of this actor
	@param templateData Template definition for the projectile actor
	@param projectileSpeed Speed of projectile movement
	@param radialOffsetRatio Position the projectile some distance away from
	                         the up axis of the actor, using some ratio of the
							 length of the radius.
	@param damageModifier Damage modifier to use for the projectile.
	                      Specify 1.0 to use the value from the template file
						  without modification
	*/
	void spawnProjectile(const vec3 &direction,
	                     const ComponentPhysics &rigidBody,
	                     const PropertyBag &templateData,
	                     const float projectileSpeed,
						 const float radialOffsetRatio = 2.0f,
						 float damageModifier = 1.0f);

	/**
	Creates a particle system
	@param fileName Particle system filename
	@param position Particle system position
	@param rotation Rotation of the system about the Z-axis (radians)
	*/
	void spawnParticleSystem(const FileName &fileName,
	                         const vec3 &position,
	                         float rotation);

	/**
	Relays a message to the actor specified
	@param id ID of the actor to direct the message towards
	@param message Message to relay
	*/
	void relayMessage(OBJECT_ID id, Message &message);

	/** Gets the movement component from the actor that owns this weapon */
	ComponentMovement& getMovementComponent();

	/** Gets the physics component from the actor that owns this weapon */
	ComponentPhysics& getPhysicsComponent();

protected:
	/** List of possible sounds when firing */
	ListBag<FileName> soundFileNames;

	/** Data describing the projetcile */
	PropertyBag projectileData;

	/** Speed of the projectile */
	float projectileSpeed;

private:
	/** Game World */
	World *world;

	/** Object that owns this weapon */
	ActorPtr actor;

	/** Time allowed to pass within the current state */
	float timeInState;

	/** Time that must pass before the weapon may fire again */
	float coolDownTime;

	/** Indicates that whether a charge up is possible */
	bool canChargeUp;

	/** Weapon state */	
	STATE state;
};

#endif
