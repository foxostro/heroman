#ifndef MESSAGE_CHARACTER_UPGRADES_WEAPON_H
#define MESSAGE_CHARACTER_UPGRADES_WEAPON_H

#include "BlackBoard.h"
#include "Actor.h"

/**
Message to notify that a character has met requirements
to upgrade their weapon
*/
class MessageCharacterUpgradesWeapon : public Message
{
public:
	MessageCharacterUpgradesWeapon() 
	: Message("MessageCharacterUpgradesWeapon")
	{}
};

/**
Message to notify that a character has met conditions
to force weapon downgrade
*/
class MessageCharacterDowngradesWeapon : public Message
{
public:
	MessageCharacterDowngradesWeapon() 
		: Message("MessageCharacterDowngradesWeapon")
	{}
};

#endif
