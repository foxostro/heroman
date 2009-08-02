#include "stdafx.h"
#include "World.h"
#include "SDLinput.h"
#include "MessageClientUsesObject.h"
#include "MessageSetWorld.h"
#include "MessageZombifyActor.h"
#include "MessageCharacterHasDied.h"
#include "ComponentDestroySelfOnCollision.h"

ComponentDestroySelfOnCollision::
ComponentDestroySelfOnCollision(BlackBoardUID uid, BlackBoard *blackBoard)
: world(0),
  shouldActivate(false),
  onlyPlayers(false),
  Component(uid, blackBoard)
{
	registerMessageHandler("MessagePhysicsCollision", bind(&ComponentDestroySelfOnCollision::handleMessagePhysicsCollision, this, _1));
	registerMessageHandler("MessageSetWorld",         bind(&ComponentDestroySelfOnCollision::handleMessageSetWorld, this, _1));
}

void ComponentDestroySelfOnCollision::load(const PropertyBag &data)
{
	resetMembers();
	data.get("onlyPlayers", onlyPlayers); // optional tag
}

void ComponentDestroySelfOnCollision::resetMembers()
{
	shouldActivate = false;
	onlyPlayers = false;
}

void ComponentDestroySelfOnCollision::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
}

bool ComponentDestroySelfOnCollision::isPlayer(Actor *actor) const
{
	vector<ActorPtr> &players = world->players;

	for(vector<ActorPtr>::iterator i = players.begin();
		i != players.end(); ++i)
	{
		if((*i)->getUID() == actor->getUID())
		{
			return true;
		}
	}

	return false;
}

void ComponentDestroySelfOnCollision::handleMessagePhysicsCollision(Message &message)
{
	if(!onlyPlayers)
	{
		shouldActivate=true;
		return;
	}

	dGeomID o2 = message.getField<dGeomID>("o2");
	Actor *a2 = reinterpret_cast<Actor*>(dGeomGetData(o2));

	if(a2 && isPlayer(a2))
	{
		dGeomID o1 = message.getField<dGeomID>("o1");
		Actor *a1 = reinterpret_cast<Actor*>(dGeomGetData(o1));

		OBJECT_ID uid = getActorPtr()->getUID();

		if(a1 && a1->getUID() == uid)
		{
			shouldActivate=true;
		}
	}
}

void ComponentDestroySelfOnCollision::update(float)
{
	if(shouldActivate)
	{
		getParentBlackBoard().relayMessage(MessageCharacterHasDied());
		getParentBlackBoard().relayMessage(MessageZombifyActor());
	}
}
