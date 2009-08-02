#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentMovement.h"
#include "WeaponItem_ChargeShot.h"

WeaponItem_ChargeShot::WeaponItem_ChargeShot(World *_world,
                                             ActorPtr _actor)
: WeaponItem(_world, _actor),
  chargePeriod(250.0f),
  maxCharge(5.0f)
{}

void WeaponItem_ChargeShot::onLoad(const PropertyBag &data)
{
	chargePeriod = data.getFloat("chargePeriod");
	maxCharge = data.getFloat("maxCharge");
}

void WeaponItem_ChargeShot::onFire()
{
	ComponentPhysics& rigidBody = getPhysicsComponent();
	ComponentMovement& movement = getMovementComponent();

	playSound(soundFileNames.getRandom());

	float damageModifier = min(1.0f+timeCharged/chargePeriod, maxCharge);

	spawnProjectile(movement.getFacingVector(),
	                rigidBody,
	                projectileData,
	                projectileSpeed,
	                2.0f,
					damageModifier);
}

WeaponItem::STATE WeaponItem_ChargeShot::onUpdateCharge(float milliseconds)
{
	timeCharged = getTimeInState();
	return CHARGING;
}
