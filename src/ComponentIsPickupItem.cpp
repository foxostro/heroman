#include "stdafx.h"
#include "world.h"
#include "MessageCharacterPicksUpItem.h"
#include "MessageZombifyActor.h"
#include "ComponentIsPickupItem.h"
#include "PickupItem_Health.h"
#include "PickupItem_Coin.h"
#include "PickupItem_Power.h"

ComponentIsPickupItem::~ComponentIsPickupItem()
{
	delete effects;
}

ComponentIsPickupItem::ComponentIsPickupItem(BlackBoardUID uid, 
											 BlackBoard *blackBoard) 
: world(0),
  effects(0),
  activated(false),
  Component(uid, blackBoard)
{
	registerMessageHandler("MessageClientUsesObject", bind(&ComponentIsPickupItem::handleMessageClientUsesObject, this, _1));
	registerMessageHandler("MessageSetWorld",         bind(&ComponentIsPickupItem::handleMessageSetWorld, this, _1));

	stringToPType["Health"] = Pickup_Health;
	stringToPType["Power"] = Pickup_Power;
	stringToPType["Coin"] = Pickup_Coin;

	resetMembers();
}

void ComponentIsPickupItem::resetMembers()
{
	type = Pickup_Coin;

	delete effects;
	effects = 0;

	activated = false;
}

PickupType ComponentIsPickupItem::getPickupTypeFromName(const string &s) const
{
	map<string, PickupType>::const_iterator i = stringToPType.find(s);
	VERIFY(i != stringToPType.end(), "Cannot identify pickup type: " + s);
	return i->second;
}

void ComponentIsPickupItem::load(const PropertyBag &data)
{
	resetMembers();

	type = getPickupTypeFromName(data.getString("pickupType"));

	switch(type)
	{
	case Pickup_Health: effects = new PickupItem_Health(world); break;
	case Pickup_Power:  effects = new PickupItem_Power(world); break;
	case Pickup_Coin:   effects = new PickupItem_Coin(world); break;
	}

	effects->load(data.getBag("effects"));
}

void ComponentIsPickupItem::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
}

void ComponentIsPickupItem::handleMessageClientUsesObject(Message &m)
{
	ASSERT(effects, "Member was null: effects");

	const OBJECT_ID requesterID = m.getField<OBJECT_ID>("requesterID");

	effects->executeEffect(requesterID);

	{MessageCharacterPicksUpItem m2(requesterID,type);
	getParentBlackBoard().relayGlobalMessage(m2);}

	/*
	we really don't want to be freeing reosurces within a collision
	callback handler, so flag that this actor should be destroyed at
	the next tick.
	*/
	activated=true;
}

void ComponentIsPickupItem::update(float milliseconds)
{
	if(activated)
	{
		getParentBlackBoard().relayMessage(MessageZombifyActor());
	}
}
