#include "stdafx.h"
#include "world.h"
#include "ActorSet.h"
#include "PropertyBag.h"
#include "MessageCharacterHasDied.h"
#include "ComponentHealth.h"
#include "ComponentPhysics.h"
#include "ComponentMonsterSpawn.h"

/* -- Functions -- */

/** Constructor */
ComponentMonsterSpawn::ComponentMonsterSpawn(BlackBoardUID uid, 
                                             BlackBoard *blackBoard) 
: Component(uid, blackBoard)
{
	resetMembers();

	/** Register message handler callbacks */
	registerMessageHandler("MessageSetWorld",                bind(&ComponentMonsterSpawn::handleMessageSetWorld, this, _1));
	registerMessageHandler("MessageCharacterHasDied",        bind(&ComponentMonsterSpawn::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageClientApproachesObject",  bind(&ComponentMonsterSpawn::handleMessageClientApproachesObject, this, _1));
	registerMessageHandler("MessageClientRecedesFromObject", bind(&ComponentMonsterSpawn::handleMessageClientRecedesFromObject, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",      bind(&ComponentMonsterSpawn::handleMessagePositionHasBeenSet, this, _1));
}

/** Destructor */
ComponentMonsterSpawn::~ComponentMonsterSpawn(void)
{
}

/** Resets the data associated with this monster spawn point to default values */
void ComponentMonsterSpawn::resetMembers()
{
	timeSinceLastSpawn = 0.0;
	totalSpawned = 0;
	spawnRate = 0.0;
	maxSpawnNum = 0;
	maxSimulNum = 0;
	bKillChildrenOnDeath = false;
	bSpawnProximityOnly = false;
	bPlayerInProximity = false;
	bIsOn = false;
	spawnInterval = 0.0;
	children.clear();
	lastReportedPosition.zero();
}

void ComponentMonsterSpawn::draw() const
{
	/* Does nothing */
}

/** Loads necessary data to construct monster spawn point */
void ComponentMonsterSpawn::load(const PropertyBag &data)
{
	//clear data before loading
	resetMembers();

	//load appropriate data
	spawnRate = data.getFloat("spawnRate");
	//typeToSpawn = getMonsterTypeFromString(data.getString("typeToSpawn"));
	maxSpawnNum = data.getInt("maxSpawnNum");
	maxSimulNum = data.getInt("maxSimulNum");
	bKillChildrenOnDeath = data.getBool("bKillChildrenOnDeath");
	bSpawnProximityOnly = data.getBool("bSpawnProximityOnly");
	bIsOn = data.getBool("bIsOn");
	templateFile = data.getFileName("monsterTemplateFile");

	//calculate spawn interval (in milliseconds) and set time
	spawnInterval = (60000.0f) / spawnRate;
	timeSinceLastSpawn = 0.0f;
}

/** Spawns monsters at appropriate intervals if necessary conditions are met */
void ComponentMonsterSpawn::update(float milliseconds)
{
	timeSinceLastSpawn += milliseconds;

	removeDeadChildren();

	if(timeSinceLastSpawn >= spawnInterval && spawnOK())
	{
		timeSinceLastSpawn = 0.0f;
		children.insert(children.begin(), spawnMonster());
	}
}

/** Determines if monster can be spawned at the moment */
bool ComponentMonsterSpawn::spawnOK()
{
	if (maxSpawnNum>=0 && totalSpawned >= maxSpawnNum)
	{
		// lifetime spawn limit reached
		return false;
	}
	else if (children.size() >= (unsigned int)maxSimulNum && maxSimulNum >= 0)
	{
		// max simultaneous monsters reached
		return false;
	}
	else if (bSpawnProximityOnly)
	{
		return bIsOn && bPlayerInProximity;	//check for player proximity
	}	
	else
	{
		return bIsOn;
	}
}

/** Spawns a monster at this spawn point and returns a pointer to it */
ActorPtr ComponentMonsterSpawn::spawnMonster()
{
	return spawnMonster(loadActorDefinition(templateFile),
	                    generateSpawnLocation());
}

ActorPtr ComponentMonsterSpawn::spawnMonster(const ComponentDataSet &s,
											 const vec3 &spawnLoc)
{
	ASSERT(world, "world has not been set yet");
	const tuple<OBJECT_ID, ActorPtr> t = world->getObjects().create();
	ActorPtr newMonster = t.get<1>();
	ASSERT(newMonster, "Failed to create monster actor");
	newMonster->load(s, spawnLoc, vec3(0.0, 0.0, 0.0), world);
	newMonster->enableDebugRendering(displayDebugData);

	totalSpawned++;
	return newMonster;
}

/** 
Callback handler for MessageSetWorld. Sets reference to current world.
*/
void ComponentMonsterSpawn::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
}

/** 
Callback handler for MessageCharacterHasDied. Handles behavior for when
spawn point has been destroyed.
*/
void ComponentMonsterSpawn::handleMessageCharacterHasDied(Message &m)
{
	//if spawn point is set to destroy all children on its death,
	//iterate through list of children and kill them all, then clear list
	if (bKillChildrenOnDeath)
	{
		list<ActorPtr>::iterator iter;
		for (iter = children.begin(); iter != children.end(); iter++)
		{
			(*iter)->relayMessage(MessageCharacterHasDied());
		}
	}

	children.clear();
	bIsOn = false;	//turn off spawn point on death
}

/**
Callback handler for MessageClientApproachesObject. Sets player being within
a certain maximum activation distance.
*/
void ComponentMonsterSpawn::handleMessageClientApproachesObject(Message&)
{
	bPlayerInProximity = true;
}

/**
Callback handler for MessageClientRecedesFromObject. Sets player being outside
a certain maximum activation distance.
*/
void ComponentMonsterSpawn::handleMessageClientRecedesFromObject(Message&)
{
	bPlayerInProximity = false;
}

void ComponentMonsterSpawn::removeDeadChildren()
{
	/*
	list<ActorPtr>::iterator iter = children.begin();
	while(iter != children.end())
	{
		ActorPtr actor = *iter;
		Component *c = actor->getComponent("Health");
		ComponentHealth *h = dynamic_cast<ComponentHealth*>(c);

		if(!h || (h && h->isDead()))
		{
			iter = children.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	*/
}

ComponentDataSet ComponentMonsterSpawn::loadActorDefinition( const FileName &file )
{
	const PropertyBag templateData = PropertyBag::fromFile(file);
	const PropertyBag base = templateData.getBag("components");
	return ComponentDataSet::parse(base);
}

vec3 ComponentMonsterSpawn::generateSpawnLocation() const
{
	const ComponentPhysics &r =
		getPrerequisiteComponent<ComponentPhysics>("Physics");

	vec2 p;
	float height = 0.0f;

	const Map &m = world->getMap();

	float bufferZone = 1.5f;
	float minRadius = r.getRadius() * bufferZone;
	float maxRadius = minRadius*2.0f;

	do 
	{
		float radius = FRAND_RANGE(minRadius, maxRadius);
		float angle = FRAND_RANGE(0.0f, (float)M_PI / 2.0f);
		vec2 offset = vec2(radius * cosf(angle), radius * sinf(angle));	

		p = offset + lastReportedPosition.xy();

		const ivec2 tilePos = m.toTileSpace(p);
		const Tile &tile = m.getTile(tilePos);
		height = tile.getTileHeight();
	} while(height > 0.2f);

	return vec3(p, 0.2f);
}
