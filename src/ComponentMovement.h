#ifndef COMPONENT_MOVEMENT_H
#define COMPONENT_MOVEMENT_H

#include "PropertyBag.h"
#include "Component.h"
#include "Actions.h"
#include "WeaponItem.h"
#include "DeathBehavior.h"

// prototypes
class World;
class Terrain;

enum WeaponType
{
	Weapon_PlasmaPistol,
	Weapon_TriShot,
	Weapon_GrenadeLauncher,
	Weapon_ChargeShot,
	Weapon_Enemy_PlasmaPistol
};

/**
Allows movement within the game world.
Requires a ComponentPhysics to be attached to the object
*/
class ComponentMovement : public Component
{
public:
	virtual string getTypeString() const { return "Movement"; }

	ComponentMovement(BlackBoardUID _uid, BlackBoard *_blackBoard);

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Declares the position and orientation for the frame */
	virtual void update(float milliseconds);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Gets the angle (radians) that the character is currently facing */
	float getFacingAngle() const
	{
		return facingAngle;
	}

	/** Gets the direction to attack towards */
	vec3 getFacingVector() const
	{
		return vec3(getDirectionVector(getFacingAngle()), 0.0f);
	}

	/** Sets the top speed for this actor */
	inline void setTopSpeed(float speed)
	{
		topSpeed = speed;
	}

	/** Gets the top speed for this actor */
	inline float getTopSpeed()
	{
		return topSpeed;
	}

private:
	void handleMessageCharacterHasBeenResurrected(Message &message);
	void handleMessageCharacterHasDied(Message &message);
	void handleMessagePerformAction(Message &_message);
	void handleMessageSetWorld(Message &_message);
	void handleMessageRequestCharacterFaceAngle(Message &m);
	void handleMessageRequestCharacterTurn(Message &m);
	void handleMessageCharacterUpgradesWeapon(Message &m);
	void handleMessageCharacterDowngradesWeapon(Message &m);
	void handleMessagePhysicsDisable(Message &message);
	void handleMessagePhysicsEnable(Message &message);


	/** Create motor joints to facilitate  the character movement now that */
	void createMotorJoints();

	/** Attack action (e.g. fire gun) */
	void endChargeUp();
	
	/** Weapon / ability charge action */
	void chargeUp();

	/** Fires the weapon without any sort of charging non-sense */
	void attackOnce();

	/** Commit suicide (perhaps for testing purposes) */
	void suicide();

	/** Applies some action to the object */
	void applyAction(Action action);

	/** Walk a step in some direction */
	void walk(const vec2 &direction, float speed=1.0f);

	/** Determines the most appropriate walk animation */
	string determineCurrentAnim() const;

	template<typename TYPE> static inline
	TYPE sign(TYPE x)
	{
		static const TYPE zero(0);
		static const TYPE one(1);
		static const TYPE neg(-1);
		return x==zero ? zero : ( x>zero ? one : neg );
	}

	/**
	Gets the unit vector pointing towards a particular angle
	@param angle radians
	*/
	static vec2 getDirectionVector(float angle)
	{
		return vec2(cosf(angle), sinf(angle));
	}

	/**
	Turns the character to face a particular direction
	@param angle radians
	*/
	void turnToFace(float angle)
	{
		if(dead) return;
		facingAngle = angle;
	}

	/**
	Turns the character by some amount
	@param dAngle radians
	*/
	void turn(float dAngle);

	/**
	Turns the character to face a particular direction
	@param direction Vector poibts in the direction to face
	*/
	void turnToFace(const vec2 &direction)
	{
		turnToFace(getAngle(direction, vec2(1,0)));
	}

	/** Gets the weapon type from a string that identifies it */
	WeaponType getWeaponTypeFromName(const string &s) const;

	/** Loads all weapon data, a helper for the load method.*/
	void loadWeapons(const PropertyBag &data);

	/** Loads weapon data for a single weapon, a helper for the load method.*/
	shared_ptr<WeaponItem> loadWeapon(const PropertyBag &data);

	/**
	Creates a new weapon from name
	@param weaponName Weapon name
	@param world Game world
	@return newly constructed weapon
	*/
	shared_ptr<WeaponItem> weaponFactory(const string &weaponName) const;
public:
	/** Gets the current weapon */
	shared_ptr<WeaponItem> getWeapon();
private:
	/** Gets the death behavior of the character */
	DeathBehavior getDeathBehavior() const;

private:
	/** Game world */
	World *world;

	/** Connection to the physics engine */
	shared_ptr<PhysicsEngine> physicsEngine;

	/** Physics joint used for character locomotion */
	dJointID amotor;

	/** Physics joint used for character locomotion */
	dJointID lmotor;

	/** Top speed of the character (meters/second)*/
	float topSpeed;

	/** Last action taken */
	Action lastAction;

	/** Most recent velocity of the character */
	vec3 velocity;

	/** Angle that the character is rotated about the Z-axis */
	float facingAngle;

	/** indicates that the player is alive (false) or dead (true) */
	bool dead;

	/** Index of the character's weapon */
	int currentWeapon;

	/** All weapons on the character's upgrade path */
	vector<shared_ptr<WeaponItem> > weapons;

	/** string -> PickupType */
	map<string, WeaponType> stringToWType;

	/** Indicates that physics have been disabled */
	bool disablePhysics;
};

#endif
