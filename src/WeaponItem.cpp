#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "ComponentMovement.h"
#include "WeaponItem.h"

WeaponItem::WeaponItem(World *_world, ActorPtr _actor)
: world(_world),
  actor(_actor),
  timeInState(0.0f),
  state(READY)
{
	ASSERT(world, "Null member: world");
	ASSERT(actor, "Null member: actor");
}

void WeaponItem::playSound(const FileName &fileName) const
{
	BlackBoard &bb = getWorld().getParentBlackBoard();
	bb.relayGlobalMessage(MessagePlaySound(fileName));
}

void WeaponItem::relayMessage(OBJECT_ID id, Message &message)
{
	getWorld().getObjects().get(id).relayMessage(message);
}

void WeaponItem::spawnProjectile(const vec3 &direction,
								 const ComponentPhysics &rigidBody,
								 const PropertyBag &templateData,
								 const float projectileSpeed,
								 const float radialOffsetRatio,
								 float damageModifier)
{
	ASSERT(world, "Null pointer: world");

	vec3 velocity = direction * projectileSpeed;
	float radialOffset = rigidBody.getRadius() * radialOffsetRatio;
	vec3 position = rigidBody.getPosition() + direction*radialOffset;

	// Parse projectile definition
	const PropertyBag base = templateData.getBag("components");
	ComponentDataSet s = ComponentDataSet::parse(base);

	// Specifically set projectile damage
	if(damageModifier != 1.0f)
	{
		PropertyBag &damageOnCollision = s.get("DamageOnCollision");
		int baseDamage = damageOnCollision.getInt("damage");
		damageOnCollision.remove("damage", 0);
		damageOnCollision.add("damage", baseDamage * damageModifier);
	}

	// Spawn the projectile
	world->getObjects().spawn(s, position, velocity);
}

void WeaponItem::spawnParticleSystem(const FileName &fileName,
									 const vec3 &position,
									 float rotation)
{
	ASSERT(world, "Null pointer: world");

	TextureFactory &textureFactory = world->getTextureFactory();
	
	world->particleEngine.add(fileName,
	                          position,
	                          rotation,
	                          textureFactory);
}

ComponentMovement& WeaponItem::getMovementComponent()
{
	ASSERT(actor, "Null member: actor");
	ComponentMovement* movement =
		dynamic_cast<ComponentMovement*>(actor->getComponent("Movement"));
	ASSERT(movement, "Actor does not contain movement information");
	return *movement;
}

ComponentPhysics& WeaponItem::getPhysicsComponent()
{
	ASSERT(actor, "Null member: actor");
	ComponentPhysics* rigidBody =
		dynamic_cast<ComponentPhysics*>(actor->getComponent("Physics"));
	ASSERT(rigidBody, "Actor does not contain physics information");
	return *rigidBody;
}

void WeaponItem::load(const PropertyBag &data)
{
	state = READY;
	coolDownTime = data.getFloat("coolDownTime");
	canChargeUp = data.getBool("canChargeUp");
	soundFileNames = data.getListBag<FileName>("sfx");
	projectileData = data.getBag("projectileData");
	projectileSpeed = data.getFloat("projectileSpeed");
	onLoad(data);
}

void WeaponItem::update(float milliseconds)
{
	STATE newState;
	timeInState += milliseconds;

	switch(state)
	{
	case READY:
		newState = onUpdateReady(milliseconds);
		break;

	case CHARGING:
		newState = onUpdateCharge(milliseconds);
		break;

	case COOLDOWN:
		if(timeInState >= coolDownTime)
		{
			newState = READY;
		}
		else
		{
			newState = onUpdateCooldown(milliseconds);
		}
		break;
	}

	// State transitions
	if(newState != state)
	{
		state = newState;
		timeInState = 0.0f;
	}
}

void WeaponItem::endChargeUp()
{
	if(state==CHARGING)
	{
		onFire();
		state = COOLDOWN;
		timeInState = 0;
	}
}

void WeaponItem::beginChargeUp()
{
	if(state==READY)
	{
		if(canChargeUp)
		{
			state = CHARGING;
			timeInState = 0;
		}
		else
		{
			onFire();
			state = COOLDOWN;
			timeInState = 0;
		}
	}
}
