#ifndef COMPONENT_MONSTER_SPAWN_H
#define COMPONENT_MONSTER_SPAWN_H

#include "PropertyBag.h"
#include "Component.h"
#include "Actor.h"

/**
This component describes a spawn point that generates monsters on the
map at a given rate. When it is destroyed it stops spawning monsters.
*/
class ComponentMonsterSpawn : public Component
{
public:
	/* -- Functions -- */
	/** Constructor */
	ComponentMonsterSpawn(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Destructor */
	~ComponentMonsterSpawn(void);

	/** Returns type of component in string */
	virtual string getTypeString() const { return "MonsterSpawn"; }

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Updates the object */
	virtual void update(float milliseconds);

private:
	/* -- Functions -- */
	/** Message handlers */
	void handleMessageSetWorld(Message &m);
	void handleMessageCharacterHasDied(Message &m);
	void handleMessageClientApproachesObject(Message &m);
	void handleMessageClientRecedesFromObject(Message &m);
	void handleMessagePositionHasBeenSet(Message &message)
	{
		lastReportedPosition = message.getField<vec3>("position");
	}

	/**
	Spawns a monster at this spawn point
	@return returns a pointer to the newly spawned monster
	*/
	ActorPtr spawnMonster();

	/**
	Spawns a monster given its actor definition and a location
	@param s actor data definition
	@param spawnLoc position to spawn at
	@return returns a pointer to the newly spawned monster
	@todo get a list of unoccupied floor tiles from the map, select
	      spawn point from this list
	@todo Sound effect and particle effects
	*/
	ActorPtr spawnMonster(const ComponentDataSet &s,
	                      const vec3 &spawnLoc);

	/**
	Chooses a position to spawn the monster at
	@return spawn point position
	*/
	vec3 generateSpawnLocation() const;

	/**
	Loads the actor definition from file
	@param file File that contains the actor definition
	@return actor data definition
	*/
	static ComponentDataSet loadActorDefinition(const FileName &file);

	/** Determines if monster is allowed to be spawned */
	bool spawnOK();

	/** Removes all dead children from the list of spawned children */
	void removeDeadChildren();

private:
	/* -- Instance Variables -- */
	list<ActorPtr> children;	/** list of pointers to all the (alive) monsters spawned by this spawn point */
	int totalSpawned;			/** the total number of monsters spawned */
	World *world;				/** reference to game world */
	float spawnRate;			/** rate at which monsters are spawned (# monsters/minute) */
	FileName templateFile;		/** the template file for the monster */
	int maxSpawnNum;			/** maximum # of monsters to spawn (-1 for no limit) */
	int maxSimulNum;			/** maximum # of monsters spawned from this point that can exist simultaneously (-1 for no limit) */
	bool bKillChildrenOnDeath;	/** spawn point destroys all spawned monsters when it is destroyed */
	bool bSpawnProximityOnly;	/** spawn point only spawns monsters while player is w/in a certain distance */
	bool bPlayerInProximity;    /** whether player is within minimum activation distance of this spawn point */
	bool bIsOn;					/** toggles spawn point on or off */
	float spawnInterval;		/** time between monster spawns (in milliseconds) */
	float timeSinceLastSpawn;	/** time since the last monster was spawned */
	vec3 lastReportedPosition;
};

#endif
