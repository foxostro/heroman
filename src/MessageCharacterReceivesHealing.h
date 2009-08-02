#ifndef MESSAGE_CHARACTER_RECEIVES_HEALING_H
#define MESSAGE_CHARACTER_RECEIVES_HEALING_H

#include "BlackBoard.h"
#include "Actor.h"

/** Message to notify that the specified character has received healing */
class MessageCharacterReceivesHealing : public Message
{
public:
	MessageCharacterReceivesHealing(int healing)
		: Message("MessageCharacterReceivesHealing")
	{
		addField("healing", healing);
	}
};

#endif
