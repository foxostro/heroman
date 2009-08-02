#ifndef MESSAGE_CHARACTER_RECEIVES_WEAPON_POWER_H
#define MESSAGE_CHARACTER_RECEIVES_WEAPON_POWER_H

#include "BlackBoard.h"

/**
Message to notify that the specified character has received
additional weapon power
*/
class MessageCharacterReceivesWeaponPower : public Message
{
public:
	MessageCharacterReceivesWeaponPower(int power)
		: Message("MessageCharacterReceivesWeaponPower")
	{
		addField("power", power);
	}
};

/**
Message to notify that the specified character has lost
some weapon power
*/
class MessageCharacterLosesWeaponPower : public Message
{
public:
	MessageCharacterLosesWeaponPower(int power)
		: Message("MessageCharacterLosesWeaponPower")
	{
		addField("power", power);
	}
};

#endif
