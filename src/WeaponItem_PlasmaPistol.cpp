#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentMovement.h"
#include "WeaponItem_PlasmaPistol.h"

WeaponItem_PlasmaPistol::WeaponItem_PlasmaPistol(World *_world,
												 ActorPtr _actor)
: WeaponItem(_world, _actor)
{}

void WeaponItem_PlasmaPistol::onLoad(const PropertyBag &data)
{}

void WeaponItem_PlasmaPistol::onFire()
{
	ComponentPhysics& rigidBody = getPhysicsComponent();
	ComponentMovement& movement = getMovementComponent();

	playSound(soundFileNames.getRandom());

	spawnProjectile(movement.getFacingVector(),
	                rigidBody,
	                projectileData,
	                projectileSpeed,
	                2.0f);
}
