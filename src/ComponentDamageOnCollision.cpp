#include "stdafx.h"
#include "World.h"
#include "SDLinput.h"
#include "MessageCharacterReceivesDamage.h"
#include "MessageSetWorld.h"
#include "ComponentDamageOnCollision.h"

ComponentDamageOnCollision::ComponentDamageOnCollision(BlackBoardUID uid, 
												       BlackBoard *blackBoard)
: world(0),
  onlyPlayers(false),
  Component(uid, blackBoard)
{
	registerMessageHandler("MessagePhysicsCollision", bind(&ComponentDamageOnCollision::handleMessagePhysicsCollision, this, _1));
	registerMessageHandler("MessageSetWorld",         bind(&ComponentDamageOnCollision::handleMessageSetWorld, this, _1));
}

void ComponentDamageOnCollision::load(const PropertyBag &data)
{
	resetMembers();
	onlyPlayers = data.getBool("onlyPlayers");
	damage = data.getInt("damage");
}

void ComponentDamageOnCollision::resetMembers()
{
	onlyPlayers = false;
	damage = 100;
}

void ComponentDamageOnCollision::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
}

bool ComponentDamageOnCollision::isPlayer(Actor *actor) const
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

void ComponentDamageOnCollision::handleMessagePhysicsCollision(Message &message)
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
			ActorSet &s = world->getObjects();

			s.get(a2->getUID()).relayMessage(MessageCharacterReceivesDamage(damage));
		}
	}
}
