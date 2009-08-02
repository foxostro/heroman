#ifndef _ACTOR_FACTORY_H_
#define _ACTOR_FACTORY_H_

#include "UniqueIdFactory.h"
#include "Actor.h"

class ActorFactory
{
public:
	typedef map<OBJECT_ID, ActorPtr> MapHandleToObject;

	/** Stores objects that have been allocated */
	MapHandleToObject objects;

private:
	/** Generates unique names for objects */
	UniqueIdFactory<OBJECT_ID> nameFactory;

public:
	/** Default constructor */
	ActorFactory()
	: nameFactory(100)
	{}

	/** Destructor */
	~ActorFactory()
	{
		destroy();
	}

	/** Cleanly destroys the objects before clearing the records of them */
	void destroy()
	{
		objects.clear();
	}

	/**
	Removes a single object from the database
	@param handle ID of the object to delete
	*/
	void remove(OBJECT_ID handle)
	{
		MapHandleToObject::iterator i;

		i = objects.find(handle);

		ASSERT(i != objects.end(), "Could not find object with handle: " + itos(handle));
		ASSERT(i->first == handle,
               "Handle of retrieved object (" + itos(i->first) + ") "
               "is different than expected (" + itos(handle) + ")");

		delete i->second;
		objects.erase(i);
	}

	/**
	Creates an actor
	@return tuple with the objects id (1st) and a pointer to the object (2nd)
	*/
	tuple<OBJECT_ID, ActorPtr> create()
	{
		OBJECT_ID uid = nameFactory.getUid();
		ActorPtr object = ActorPtr(new Actor(uid));
		ASSERT(object, "Allocator failed");
		objects.insert(make_pair(uid, object));
		return make_tuple(uid, object);
	}
};

ActorFactory& getActorFactory();

#endif
