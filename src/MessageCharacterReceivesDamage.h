#ifndef MESSAGE_CHARACTER_RECEIVES_DAMAGE_H
#define MESSAGE_CHARACTER_RECEIVES_DAMAGE_H

#include "BlackBoard.h"
#include "Actor.h"

/** Message to notify that the specified character has received damage */
class MessageCharacterReceivesDamage : public Message
{
public:
	MessageCharacterReceivesDamage(int damage)
		: Message("MessageCharacterReceivesDamage")
	{
		addField("damage", damage);
	}
};

#endif
