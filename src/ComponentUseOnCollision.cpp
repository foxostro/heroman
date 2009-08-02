#include "stdafx.h"
#include "World.h"
#include "SDLinput.h"
#include "MessageClientUsesObject.h"
#include "MessageSetWorld.h"
#include "ComponentUseOnCollision.h"

ComponentUseOnCollision::ComponentUseOnCollision(BlackBoardUID uid, 
												 BlackBoard *blackBoard)
: world(0),
  onlyPlayers(false),
  Component(uid, blackBoard)
{
	registerMessageHandler("MessagePhysicsCollision", bind(&ComponentUseOnCollision::handleMessagePhysicsCollision, this, _1));
	registerMessageHandler("MessageSetWorld",         bind(&ComponentUseOnCollision::handleMessageSetWorld, this, _1));
}

void ComponentUseOnCollision::load(const PropertyBag &data)
{
	resetMembers();
	onlyPlayers = data.getBool("onlyPlayers");
}

void ComponentUseOnCollision::resetMembers()
{
	onlyPlayers = false;
}

void ComponentUseOnCollision::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
}

bool ComponentUseOnCollision::isPlayer(Actor *actor) const
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

void ComponentUseOnCollision::handleMessagePhysicsCollision(Message &message)
{
	dGeomID o2 = message.getField<dGeomID>("o2");
	Actor *a2 = reinterpret_cast<Actor*>(dGeomGetData(o2));

	if(a2 && (!onlyPlayers || (onlyPlayers && isPlayer(a2))))
	{
		dGeomID o1 = message.getField<dGeomID>("o1");
		Actor *a1 = reinterpret_cast<Actor*>(dGeomGetData(o1));

		OBJECT_ID uid = getActorPtr()->getUID();

		if(a1 && a1->getUID() == uid)
		{
			dContact c = message.getField<dContact>("contact");
			getParentBlackBoard().relayMessage(MessageClientUsesObject(uid));
		}
	}
}
