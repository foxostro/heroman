#ifndef COMPONENT_HEALTH_H
#define COMPONENT_HEALTH_H

#include "PropertyBag.h"
#include "Component.h"
#include "Font.h"
#include "DeathBehavior.h"

/**
The component that holds the value of the character's health.
Tracks messages that announce damage to the character and healing applied to
the character so that it can announce whenever he has died. Very often, other
components will want to poll this one directly to determine the direct health
value and the maximum ealth value.
*/
class ComponentHealth : public Component
{
public:
	virtual string getTypeString() const { return "Health"; }

	ComponentHealth(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws any transparent visual component represented here */
	virtual void drawTransparentBits() const;

	/** Updates the object */
	virtual void update(float milliseconds);

	/** Gets the weapon power value */
	int getPower() const
	{
		return power;
	}

	/** Gets the health value */
	int getHealth() const
	{
		return health;
	}

	/** Gets the maximum health */
	int getMaxHealth() const
	{
		return maxHealth;
	}
	
	/** Is the character dead? */
	inline bool isDead() const
	{
		return dead;
	}

private:
	void handleMessagePassFont(Message &message);
	void handleMessageCharacterHasBeenResurrected(Message &message);
	void handleMessageCharacterHasDied(Message &message);
	void handleMessageCharacterReceivesDamage(Message &message);
	void handleMessageCharacterReceivesHealing(Message &message);
	void handleMessageCharacterReceivesWeaponPower(Message &message);
	void handleMessagePositionHasBeenSet(Message &message);
	void handleMessageRequestSetHeight(Message &message);
	void handleMessageExplosionOccurs(Message &message);
	void handleMessageSetWorld(Message &message);

	void losePower(int damage);
	void loseHealth(int damage);

	/** Get an actors' physics geom, or 0 */
	dGeomID getGeom(OBJECT_ID actor);

	/** Get an actors' physics geom, or 0 */
	static dGeomID getGeom(const Actor &actor);

	/**
	@param actor Actor to test the ray against
	@param p Origin of the ray in space
	@param d Direction of the ray
	@param l Ray terminates after a given length
	*/
	bool rayCast(OBJECT_ID actor, const vec3 &p, const vec3 &d, float l);

	/** Gets the death behavior of the character */
	DeathBehavior getDeathBehavior() const;

	void drawHealthBar( const mat3 &cameraOrientation ) const;

	void drawResurrectCountDown( const mat3 &cameraOrientation ) const;

private:
	World *world;
	int health;
	int maxHealth;
	int power;
	bool dead;
	bool displayPower;
	shared_ptr<Font> font;
	vec3 lastReportedPosition;
	float lastReportedHeight;
	float damageToPowerRatio;
	bool willResurrectAfterCountDown;
	float timeSpentDead;
	float timeUntilResurrection;
};

#endif
