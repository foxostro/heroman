#ifndef MESSAGE_CHARACTER_PICKS_UP_ITEM_H
#define MESSAGE_CHARACTER_PICKS_UP_ITEM_H

#include "BlackBoard.h"
#include "Actor.h"

/**
Tokens to identify possible pickups that the player may interact with.
All possible pickup items are listed here.
*/
enum PickupType
{
	Pickup_Health, // Health Pickup of any type
	Pickup_Power,  // Weapon power
	Pickup_Coin    // Coin pickup
};

/** 
Message to notify that a character has picked up an item
of the given PickupType.
*/
class MessageCharacterPicksUpItem : public Message
{
public:
	MessageCharacterPicksUpItem(OBJECT_ID id, PickupType ptype) 
	: Message("MessageCharacterPicksUpItem")
	{
		addField("id", id);
		addField("type", ptype);
	}
};

#endif
