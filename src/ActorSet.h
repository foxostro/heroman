#ifndef _ACTOR_SET_H_
#define _ACTOR_SET_H_

#include "Actor.h"
#include "ActorFactory.h"
#include "BlackBoard.h"

// class prototypes
class Frustum;
class World;

/** Collection of associated actors, by reference */
class ActorSet : public BlackBoard
{
private:
	typedef map<OBJECT_ID, ActorPtr> ActorMap;

	/** Tuple containing component data, initial position, and velocity */
	typedef tuple<ComponentDataSet, vec3, vec3> SpawnRequest;

private:
	ActorMap actors;
	queue<SpawnRequest> spawnRequests;
	World *world;
	bool displayDebugRendering;

public:
	typedef ActorMap::iterator iterator;
	typedef ActorMap::const_iterator const_iterator;

public:
	virtual string getTypeString() const { return "ActorSet"; }

	/** Creates an empty set */
	ActorSet()
	{
		world=0;
		clear();
	}

	/**
	Creates a set of objects from an XML data source
	@param xml The XML data source
	@param zone The home zone of the object
	*/
	ActorSet(const PropertyBag &data, World *world)
	{
		ASSERT(world!=0, "zone was NULL");
		clear();
		load(data, world);
	}

	inline iterator begin()
	{
		return actors.begin();
	}
	
	inline const_iterator begin() const
	{
		return actors.begin();
	}

	inline iterator end()
	{
		return actors.end();
	}

	inline const_iterator end() const
	{
		return actors.end();
	}

	inline bool getDisplayDebugRendering()
	{
		return displayDebugRendering;
	}

	/** Resets the ActorSet to an empty set */
	void clear();

	/**
	Resets the ActorSet to an empty set and explicitly requests that each object
	that was perviously within the set be destroyed.
	*/
	void destroy();

	/**
	Creates a set of objects from an XML data source
	@param xml The XML data source
	@param world The home world of the object
	*/
	void load(const PropertyBag &xml, World *world);

	/**
	Determines whether an object is a member of the ActorSet
	@param id The GUID of the object
	@return true if the object is contained within the set, false otherwise
	*/
	bool isMember(OBJECT_ID id) const;

	/**
	Creates an Actor, caches it in the global ActorFactory
	Actor is then added to the set
	@return Tuple containing the unique identifier to the new object and
	a pointer to the object itself
	*/
	tuple<OBJECT_ID, ActorPtr> create();

	/**
	Spawns an object right at the next convenient time.
	@param data Component layout and data for the object
	@param position Initial position of the object
	@param position Initial velocity of the object
	@return reference to the object
	*/
	void spawn(const ComponentDataSet &data,
	           const vec3 &position,
			   const vec3 &velocity);

	/**
	Gets an object from the set
	@param id The ID of the object to retrieve
	@return reference of the object
	*/
	Actor& get(OBJECT_ID id);

	/**
	Gets an object from the set
	@param id The ID of the object to retrieve
	@return reference of the object
	*/
	const Actor& get(OBJECT_ID id) const;

	/**
	Draws all objects in the set for the game itself.
	@param cameraFrustum The frustum of the camera
	*/
	void drawForGame(const Frustum &cameraFrustum) const;

	/**
	Draws all objects in the set for the game itself.
	@param cameraFrustum The frustum of the camera
	*/
	void drawTransparentBits(const Frustum &cameraFrustum) const;

	/**
	Update the objects
	@param deltaTime The milliseconds between now and the last tick
	*/
	void update(float deltaTime);

	/** Enable or disable debug rendering */
	void enableDebugRendering(bool enable);

	/**
	Receives a message of some kind.
	Relays a message to all blackboard subscribers.
	@param message Some message
	*/
	virtual void recvMessage(Message &message);

	/** Deletes zombie actors */
	void reapZombieActors();

private:
	/** Spawns an object right now */
	void _spawn(const SpawnRequest &data);

	/**
	Gets an object from the set
	@param id The ID of the object to retrieve
	@return reference of the object
	*/
	ActorPtr getPtr(OBJECT_ID id);

	/**
	Gets an object from the set
	@param id The ID of the object to retrieve
	@return reference of the object
	*/
	const ActorPtr getPtr(OBJECT_ID id) const;

	/**
	If the actor is within the viewing frustum, draw the actor
	@param frustum The viewing frustum
	@param p The actor in question
	*/
	static void drawActorForGame(const Frustum &frustum, Actor &p);

	/**
	If the actor is within the viewing frustum, draw the transparent bit of
	the actor
	@param frustum The viewing frustum
	@param p The actor in question
	*/
	static void drawActorTransparentBits(const Frustum &frustum, Actor &p);
};

#endif
