#ifndef ACTOR_H
#define ACTOR_H

#include "PropertyBag.h"
#include "Terrain.h"
#include "BlackBoard.h"
#include "Component.h"

class ActorSet;
class World;

typedef BlackBoardUID OBJECT_ID;
typedef size_t OBJECT_TYPE;
const OBJECT_ID INVALID_ID = 0;

/**
Structure used to create a particular actor instance.
Lists the components used by the actor and the data that is to be parsed
and loaded by each component.
*/
class ComponentDataSet
{
public:
	typedef vector<pair<string, PropertyBag> >::const_iterator const_iterator;
	typedef vector<pair<string, PropertyBag> >::iterator iterator;

private:
	vector<pair<string, PropertyBag> > data;

public:
	/** Default Constructor */
	ComponentDataSet() {}

	/** Copy Constructor */
	ComponentDataSet(const ComponentDataSet &s) : data(s.data) {}

	const_iterator begin() const
	{
		return data.begin();
	}

	const_iterator end() const
	{
		return data.end();
	}

	iterator begin()
	{
		return data.begin();
	}

	iterator end()
	{
		return data.end();
	}

	/**
	Finds data for the component, given a particular component name
	@param name Component name
	@return access to the specified componment data
	*/
	PropertyBag& get(const string &name)
	{
		for(ComponentDataSet::iterator i=begin(); i!=end(); ++i)
		{
			if(i->first == name)
			{
				return i->second;
			}
		}

		FAIL("Failed to find component: " + name);
		return end()->second;
	}

	/** Parses out component data from an object template */
	static ComponentDataSet load(const PropertyBag &base,
	                             const PropertyBag &specialization)
	{
		return merge(parse(base), parse(specialization));
	}

	/** Parses out component data from an object template */
	static ComponentDataSet parse(const PropertyBag &data)
	{
		ComponentDataSet set;

		for(size_t i=0, n=data.getNumInstances("component"); i<n; ++i)
		{
			const PropertyBag component = data.getBag("component", i);
			const string name = component.getString("name");
			set.add(name, component);
		}

		return set;
	}

	/** Parses out component data from an object template */
	static PropertyBag merge(const PropertyBag &baseData,
	                         const ComponentDataSet &specializaions,
	                         const string &name)
	{
		for(ComponentDataSet::const_iterator i=specializaions.begin();
			i!=specializaions.end(); ++i)
		{
			if(i->first == name)
			{
				PropertyBag mergedData = baseData;
				mergedData.merge(i->second, true);
				return mergedData;
			}
		}

		return baseData;
	}

	/**
	Merges two component data sets into a single set
	@param base Base data set
	@param sp Specializations are extensions and modifications to
	the base data set.  Components that are specified
	here, and are not also specified in the base, are
	ignored.
	@return Merged data set
	*/
	static ComponentDataSet merge(const ComponentDataSet &base,
	                              const ComponentDataSet &sp)
	{
		ComponentDataSet mergedSet;

		for(ComponentDataSet::const_iterator i=base.begin();
			i!=base.end();++i)
		{
			const string &name = i->first;
			const PropertyBag &baseData = i->second;
			mergedSet.add(name, merge(baseData, sp, name));
		}

		return mergedSet;
	}

private:
	void add(const string &name, const PropertyBag &componentData)
	{
		data.push_back(make_pair(name, componentData));
	}
};

/**
A basic object in the game world.  An Actor has interfaces to present a
visual representation of itself and to interact on a very basic level with
the other objects in the game world.  For example, the Actor class provide
a very simple collision detection mechanism and an event handling mechanism
that exists for a more advanced system to build upon.
*/
class Actor : public BlackBoard
{
public:
	/**
	Constructor
	@param uid The unique ID of the object
	*/
	Actor(OBJECT_ID uid);

	/** Destructor */
	virtual ~Actor();

	/** Set everything to defaults */
	virtual void clear();

	/** Free all allocated memory, cleanup */
	virtual void destroy();

	/**
	Loads the object state
	@param componentsData Component name and associated data for each component
	@param initialPosition Initial position of the object
	@param initialVelocity Initial velocity of the object
	@param world World where the object is located
	*/
	virtual void load(const ComponentDataSet &componentsData,
	                  const vec3 &initialPosition,
	                  const vec3 &initialVelocity,
	                  World*const world);

	/**
	Updates the object without displaying it
	@param deltaTime milliseconds since the last tick
	*/
	virtual void update(float deltaTime);

	/** Draws the object */
	virtual void draw() const;

	/** Draws the transparent bits of the object */
	virtual void drawTransparentBits() const;

	/** Enable or disable debug rendering */
	void enableDebugRendering(bool enable);

	/**
	Sets the world that the object is contained within
	@param world Game world
	@todo Deprecated. Replace with a blackboard Message!
	*/
	void passWorld(World*const world);
	
	/** Determines whether the actor has a particular component or not */
	inline bool hasComponent(const string &name) const
	{
		return BlackBoard::hasSubscriber(name);
	}

	/** Get actor component by name */
	ComponentPtr getComponent(const string &name);

	/** Get actor component by name */
	const ComponentPtr getComponent(const string &name) const;

	/**
	Receives a message of some kind.
	Relays a message to all blackboard subscribers.
	@param message Some message
	*/
	virtual void recvMessage(Message &message);

private:
	void handleMessageSetWorld(Message &message);
	void handleMessageZombifyActor(Message &message);

public:
	/** Indicates that the manager may delete us */
	bool zombie;

	/** Game world */
	World *world;

	/** Name given to the actor in the template file */
	string actorName;

private:
	/** Game object's components */
	vector<ComponentPtr> components;
};

typedef Actor* ActorPtr;

#endif
