#ifndef MESSAGE_CHARACTER_HAS_DIED_H
#define MESSAGE_CHARACTER_HAS_DIED_H

#include "BlackBoard.h"
#include "Actor.h"

/** Message to notify that the character has died */
class MessageCharacterHasDied : public Message
{
public:
	MessageCharacterHasDied()
	: Message("MessageCharacterHasDied")
	{}
};

#endif
