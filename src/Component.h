#ifndef COMPONENT_H
#define COMPONENT_H

#include "PropertyBag.h"
#include "BlackBoard.h"

class Component;
class Actor;
typedef Component* ComponentPtr;

class Component : public BlackBoardSubscriber
{
public:
	virtual string getTypeString() const { return "Component"; }

	Component(BlackBoardUID uid, BlackBoard *blackBoard)
	: BlackBoardSubscriber(uid, blackBoard),
	  displayDebugData(false)
	{}

	/** Resets all object members to defaults */
	virtual void resetMembers() = 0;

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data) = 0;

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds) = 0;

	/** Draws any opaque visual component represented here */
	virtual void draw() const { /* Does nothing */ }

	/** Draws any transparent visual component represented here */
	virtual void drawTransparentBits() const { /* Does nothing */ }

	/** Creates a component given the component name */
	static Component* createComponent(const string &name,
	                                  BlackBoardUID uid,
									  BlackBoard *blackBoard);

	Actor* getActorPtr() const;

protected:
	template<typename ComponentType>
	const ComponentType& getPrerequisiteComponent(const string &name) const
	{
		PrereqCache::iterator i = prerequisiteComponents.find(name);

		ComponentType *p=0;

		if(i != prerequisiteComponents.end())
		{
			p = dynamic_cast<ComponentType*>(i->second);
		}
		else
		{
			p = dynamic_cast<ComponentType*>(getParentBlackBoard().getSubscriber(name));
			prerequisiteComponents[name] = p;
		}

		ASSERT(p, "Failed to retrieve component prerequisite \"" + name + "\"");
		return *p;
	}

	template<typename ComponentType>
	ComponentType& getPrerequisiteComponent(const string &name)
	{
		PrereqCache::iterator i = prerequisiteComponents.find(name);

		ComponentType *p=0;

		if(i != prerequisiteComponents.end())
		{
			p = dynamic_cast<ComponentType*>(i->second);
		}
		else
		{
			p = dynamic_cast<ComponentType*>(getParentBlackBoard().getSubscriber(name));
			prerequisiteComponents[name] = p;
		}

		ASSERT(p, "Failed to retrieve component prerequisite \"" + name + "\"");
		return *p;
	}

private:
	template<typename COMPONENT>
	static Component* newComponent(BlackBoardUID uid,
	                               BlackBoard *blackBoard)
	{
		return new COMPONENT(uid, blackBoard);
	}

public:
	/** Indicates that any debug data should be displayed by the component */
	bool displayDebugData;

private:
	typedef map<string, function<Component* (BlackBoardUID uid, BlackBoard *blackBoard)> > AllocMap;
	typedef map<string, ComponentPtr > PrereqCache;

	/**
	Prerequisite components are cached here after they are accessed for the
	first time.
	*/
	mutable PrereqCache prerequisiteComponents;
};

#endif
