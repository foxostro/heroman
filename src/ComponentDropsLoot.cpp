#include "stdafx.h"
#include "World.h"
#include "ComponentPhysics.h"
#include "ComponentDropsLoot.h"

ComponentDropsLoot::ComponentDropsLoot(BlackBoardUID uid,
                                       BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageCharacterHasDied", bind(&ComponentDropsLoot::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageSetWorld",         bind(&ComponentDropsLoot::handleMessageSetWorld, this, _1));
}

void ComponentDropsLoot::resetMembers()
{
	lootFileNames.clear();
	maxLoots = 1;
}

void ComponentDropsLoot::load(const PropertyBag &data)
{
	resetMembers();
	lootFileNames = data.getListBag<FileName>("lootFileNames");
	maxLoots = data.getInt("maxLoots");
}

void ComponentDropsLoot::handleMessageCharacterHasDied(Message &message)
{
	int numOfLoot = IRAND_RANGE(1, maxLoots);

	for(int i=0; i<numOfLoot; ++i)
	{
		spawnLoot(lootFileNames.getRandom());
	}
}

void ComponentDropsLoot::handleMessageSetWorld( Message &message )
{
	world = message.getField<World*const>("world");
}

void ComponentDropsLoot::spawnLoot(const FileName &lootFilename)
{
	const ComponentPhysics &rigidBody =
		getPrerequisiteComponent<ComponentPhysics>("Physics");
	float projectileSpeed = FRAND_RANGE(1.0f, 8.0f);
	vec3 direction = GetRandomVector(1.0f);	
	float radialOffsetRatio = FRAND_RANGE(0.30f, 2.00f);
	float radialOffset = rigidBody.getRadius() * radialOffsetRatio;
	vec3 velocity = direction * projectileSpeed;
	vec3 position = rigidBody.getPosition() + direction*radialOffset;
	spawnLoot(lootFilename, position, velocity);
}

void ComponentDropsLoot::spawnLoot(const FileName & lootFilename,
								   const vec3 &position,
								   const vec3 &velocity)
{
	ASSERT(world, "Null pointer: world");
	const PropertyBag data = PropertyBag::fromFile(lootFilename);
	const PropertyBag base = data.getBag("components");
	ComponentDataSet s = ComponentDataSet::parse(base);

#if 1
	// Specifically enable physics for loot
	{PropertyBag &physicsBodyData = s.get("PhysicsBody");
	physicsBodyData.remove("initiallyDisablePhysics", 0);
	physicsBodyData.add("initiallyDisablePhysics", false);}
#endif

	world->getObjects().spawn(s, position, velocity);
}