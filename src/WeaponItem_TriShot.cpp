#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentMovement.h"
#include "WeaponItem_TriShot.h"

WeaponItem_TriShot::WeaponItem_TriShot(World *world, ActorPtr actor)
: WeaponItem(world, actor),
  projectileSpread(0.0f),
  coolDownTime2(0.0f),
  coolDownTimer2(0.0f),
  spreadCount(0),
  maxSpreadCount(0)
{}

void WeaponItem_TriShot::onLoad(const PropertyBag &data)
{
	projectileSpread = data.getFloat("projectileSpread");
	coolDownTime2 = data.getFloat("coolDownTime2");
	maxSpreadCount = data.getInt("maxSpreadCount");
}

void WeaponItem_TriShot::onFire()
{
	ComponentPhysics& rigidBody = getPhysicsComponent();
	ComponentMovement& movement = getMovementComponent();

	float spread = projectileSpread * (spreadCount+1) * (float)(M_PI/180.0);
	spreadCount = min(spreadCount+1, maxSpreadCount);

	vec3 direction = movement.getFacingVector();
	mat3 rot1 = mat3::fromRotateZ(spread);
	mat3 rot2 = mat3::fromRotateZ(-spread);
	vec3 direction1 = rot1.transformVector(direction);
	vec3 direction2 = rot2.transformVector(direction);

	spawnProjectile(direction,
	                rigidBody,
	                projectileData,
	                projectileSpeed,
	                2.0f);

	spawnProjectile(direction1,
	                rigidBody,
	                projectileData,
	                projectileSpeed,
	                2.0f);

	spawnProjectile(direction2,
	                rigidBody,
	                projectileData,
	                projectileSpeed,
					2.0f);

	playSound(soundFileNames.getRandom());
}

WeaponItem::STATE WeaponItem_TriShot::onUpdateReady(float milliseconds)
{
	coolDownTimer2 += milliseconds;

	if(coolDownTimer2 > coolDownTime2)
	{
		coolDownTimer2=0.0f;
		spreadCount = max(spreadCount-1, 0);
	}

	return READY;
}
