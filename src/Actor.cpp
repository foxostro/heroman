#include "stdafx.h"
#include "Font.h"
#include "World.h"
#include "Actor.h"
#include "ActorSet.h"
#include "Component.h"
#include "MessageSetWorld.h"
#include "MessageRequestPositionBeSet.h"
#include "MessagePassFont.h"

Actor::Actor(const OBJECT_ID uid)
: BlackBoard(uid, 0)
{
	clear();

	registerMessageHandler("MessageZombifyActor", bind(&Actor::handleMessageZombifyActor, this, _1));
	registerMessageHandler("MessageSetWorld",     bind(&Actor::handleMessageSetWorld, this, _1));
}

Actor::~Actor()
{
	destroy();
}

void Actor::clear()
{
	zombie = false;
	components.clear();
	BlackBoard::clear();
}

void Actor::destroy()
{
	for(vector<ComponentPtr>::iterator i = components.begin();
		i != components.end(); ++i)
	{
		delete *i;
	}

	clear();
}

void Actor::update(float milliseconds)
{
	// Update each component
	for(vector<ComponentPtr>::iterator i = components.begin();
		i != components.end(); ++i)
	{
		ComponentPtr subscriber = *i;
		ASSERT(subscriber, "Subscriber is null");
		subscriber->update(milliseconds);
	}
}

void Actor::draw() const
{
	for(vector<ComponentPtr>::const_iterator i = components.begin();
		i != components.end();
		++i)
	{
		ComponentPtr subscriber = *i;
		ASSERT(subscriber, "Subscriber is null");
		subscriber->draw();
	}
}

void Actor::drawTransparentBits() const
{
	for(vector<ComponentPtr>::const_iterator i = components.begin();
		i != components.end();
		++i)
	{
		ComponentPtr subscriber = *i;
		ASSERT(subscriber, "Subscriber is null");
		subscriber->drawTransparentBits();
	}
}

void Actor::enableDebugRendering(bool enable)
{
	for(vector<ComponentPtr>::const_iterator i = components.begin();
		i != components.end(); ++i)
	{
		ComponentPtr subscriber = *i;
		ASSERT(subscriber, "Subscriber is null");
		subscriber->displayDebugData = enable;
	}
}

void Actor::load(const ComponentDataSet &componentsData,
				 const vec3 &initialPosition,
				 const vec3 &initialVelocity,
				 World*const world)
{
	destroy();

	// List of components along-side the data they will be loading
	vector<tuple<ComponentPtr, PropertyBag> > componentsWithData;

	// Create all components before loading component data
	for(ComponentDataSet::const_iterator i = componentsData.begin();
		i != componentsData.end(); ++i)
	{
		ComponentPtr component
			= Component::createComponent(i->first,
			                             BlackBoard::genName(),
										 this);

		components.push_back(component);
		componentsWithData.push_back(make_tuple(component, i->second));
		BlackBoard::registerSubscriber(component);
	}

	relayMessage(MessageSetWorld(world));
	relayMessage(MessagePassFont(world->getFont()));

	// Have components load data
	for(vector<tuple<ComponentPtr, PropertyBag> >::iterator
			i = componentsWithData.begin();
		i != componentsWithData.end(); ++i)
	{
		ComponentPtr component = i->get<0>();
		const PropertyBag &data = i->get<1>();
		component->load(data);
	}

	// Declare the object's initial position
	relayMessage(MessageDeclareInitialPosition(initialPosition,
	                                           initialVelocity));
}

void Actor::recvMessage(Message &message)
{
	BlackBoard::recvMessage(message);
}

const ComponentPtr Actor::getComponent( const string &name ) const
{
	const SubscriberPtr s = BlackBoard::getSubscriber(name);
	const ComponentPtr componentPtr = dynamic_cast<ComponentPtr>(s);
	return componentPtr;
}

ComponentPtr Actor::getComponent( const string &name )
{
	SubscriberPtr subscriberPtr = BlackBoard::getSubscriber(name);
	ComponentPtr componentPtr = dynamic_cast<ComponentPtr>(subscriberPtr);
	return componentPtr;
}

void Actor::handleMessageSetWorld(Message &message)
{
	// Do nothing
}

void Actor::handleMessageZombifyActor(Message &message)
{
	zombie = true;
}
