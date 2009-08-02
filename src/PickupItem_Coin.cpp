#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "ComponentPhysics.h"
#include "PickupItem_Coin.h"

void PickupItem_Coin::load(const PropertyBag &data)
{
	PickupItem::load(data);
}

void PickupItem_Coin::executeEffect(OBJECT_ID id)
{
	getWorld().coinCollected();
	playSound(getSound());

	Actor &actor = getWorld().getObjects().get(id);

	ComponentPhysics* physics =
		dynamic_cast<ComponentPhysics*>(actor.getComponent("Physics"));

	if(physics)
	{
		vec3 position = physics->getPosition();

		spawnParticleSystem(FileName("data/particle/stars.xml"), position);
	}
}
