#include "stdafx.h"
#include "GraphicsDevice.h"
#include "Frustum.h"
#include "ActorFactory.h"
#include "MessageCollision.h"
#include "ComponentPhysics.h"
#include "ComponentPhysicsBody.h"
#include "ActorSet.h"

void ActorSet::clear()
{
	BlackBoard::clear();
	actors.clear();
	displayDebugRendering = false;
}

void ActorSet::destroy()
{
	// delete all actors
	while(!actors.empty())
	{
		ActorMap::iterator iter = actors.begin();
		pair<OBJECT_ID, ActorPtr> p = *iter;
		OBJECT_ID id = p.first;
		ActorPtr actor = p.second;

		removeSubscriber(id);
		getActorFactory().remove(id);
		actors.erase(iter);
	}

	displayDebugRendering = false;
}

void ActorSet::spawn(const ComponentDataSet &data,
					 const vec3 &position,
					 const vec3 &velocity)
{
	const SpawnRequest r = make_tuple(data, position, velocity);
	spawnRequests.push(r);
}

void ActorSet::_spawn(const SpawnRequest &spawnData)
{
	const ComponentDataSet &data = spawnData.get<0>();
	const vec3 &position = spawnData.get<1>();
	const vec3 &velocity = spawnData.get<2>();

	const tuple<OBJECT_ID, ActorPtr> t = create();
	const ActorPtr object = t.get<1>();
	
	object->load(data, position, velocity, world);
	object->enableDebugRendering(displayDebugRendering);
}

tuple<OBJECT_ID, ActorPtr> ActorSet::create()
{
	ActorFactory &factory = getActorFactory();
	const tuple<OBJECT_ID, ActorPtr> t = factory.create();
	const OBJECT_ID id = t.get<0>();
	const ActorPtr actor = t.get<1>();
	actors.insert(make_pair(id, actor)); // record it in this actor set
	registerSubscriber(actor);           // record it in the blackboard
	actor->setParentBlackBoard(this);
	return t;
}

const Actor& ActorSet::get(OBJECT_ID id) const
{
	ASSERT(isMember(id), "Not a member");
	return(*actors.find(id)->second);
}

Actor& ActorSet::get(OBJECT_ID id)
{
	ASSERT(isMember(id), "Not a member");
	ActorPtr actor = actors.find(id)->second;
	return(*actor);
}

const ActorPtr ActorSet::getPtr(OBJECT_ID id) const
{
	if(isMember(id))
		return(actors.find(id)->second);
	else
		return ActorPtr();
}

ActorPtr ActorSet::getPtr(OBJECT_ID id)
{
	if(isMember(id))
		return(actors.find(id)->second);
	else
		return ActorPtr();
}

void ActorSet::enableDebugRendering(bool enable)
{
	for(iterator i = begin(); i!=end(); ++i)
	{
		ActorPtr actor = i->second;

		if(actor)
		{
			actor->enableDebugRendering(enable);
		}
	}

	displayDebugRendering = enable;
}

void ActorSet::update(float deltaTime)
{
	for(iterator i = begin(); i!=end(); ++i)
	{
		ActorPtr actor = i->second;

		if(actor)
		{
			actor->update(deltaTime);
		}
	}

	while(!spawnRequests.empty())
	{
		_spawn(spawnRequests.front());
		spawnRequests.pop();
	}

	reapZombieActors();
}

void ActorSet::reapZombieActors()
{
	// delete zombie actors
	ActorMap::iterator iter = actors.begin();
	ActorMap::iterator nextIter;

	while(iter != actors.end())
	{
		// Get an iterator to the next element (or the end of the map)
		nextIter = iter;
		nextIter++;

		pair<OBJECT_ID, ActorPtr> p = *iter;
		OBJECT_ID id = p.first;
		ActorPtr actor = p.second;

		if(actor && actor->zombie)
		{
			removeSubscriber(id);
			actors.erase(id);
			getActorFactory().remove(id);
		}

		iter = nextIter;
	}
}

void ActorSet::drawActorForGame(const Frustum &frustum, Actor &actor)
{
	actor.draw();
}

void ActorSet::drawActorTransparentBits(const Frustum &frustum, Actor &actor)
{
	actor.drawTransparentBits();
}

void ActorSet::drawForGame(const Frustum &cameraFrustum) const
{
	for(const_iterator i=begin(); i!=end(); ++i)
	{
		drawActorForGame(cameraFrustum, *i->second);
	}
}

void ActorSet::drawTransparentBits(const Frustum &cameraFrustum) const
{
	for(const_iterator i=begin(); i!=end(); ++i)
	{
		drawActorTransparentBits(cameraFrustum, *i->second);
	}
}

void ActorSet::load(const PropertyBag &objects, World *_world)
{
	ASSERT(_world!=0, "world was null");
	world = _world;

	for(size_t i=0, n=objects.getNumInstances("object"); i<n; ++i)
	{
		const tuple<OBJECT_ID, ActorPtr> t = create();
		const ActorPtr object = t.get<1>();

		const PropertyBag decl = objects.getBag("object", i);
		const FileName templateFile = decl.getFileName("template");
		const vec3 initialPosition = decl.getVec3("position");
		const PropertyBag templateData = PropertyBag::fromFile(templateFile);
		const PropertyBag base = templateData.getBag("components");

		ComponentDataSet s = ComponentDataSet::load(base, decl);

		// get actor name
		object->actorName = "(no name)";
		templateData.get("name", object->actorName);

		object->load(s, initialPosition, vec3(0,0,0), world);
		object->setParentBlackBoard(this);
	}
}

bool ActorSet::isMember(OBJECT_ID id) const
{
	const_iterator i = actors.find(id);
	return (id!=INVALID_ID) && (i != actors.end());
}

void ActorSet::recvMessage(Message &message)
{
	BlackBoard::recvMessage(message);
}
