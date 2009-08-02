#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentMovement.h"
#include "WeaponItem_GrenadeLauncher.h"

WeaponItem_GrenadeLauncher::WeaponItem_GrenadeLauncher(World *_world,
													   ActorPtr _actor)
: WeaponItem(_world, _actor),
  incline(0.0f)
{}

void WeaponItem_GrenadeLauncher::onLoad(const PropertyBag &data)
{
	incline = data.getFloat("incline");
}

void WeaponItem_GrenadeLauncher::onFire()
{
	ComponentPhysics& rigidBody = getPhysicsComponent();
	ComponentMovement& movement = getMovementComponent();

	playSound(soundFileNames.getRandom());

	mat3 rotZ = mat3::fromRotateZ(movement.getFacingAngle());
	mat3 rotY = mat3::fromRotateY(incline * (float)(M_PI/180.0));
	mat3 rot = rotZ * rotY;

	vec3 direction = rot.transformVector(vec3(1,0,0));

	spawnProjectile(direction,
	                rigidBody,
	                projectileData,
	                projectileSpeed,
	                2.0f);
}
