#ifndef MESSAGE_CHARACTER_HAS_BEEN_RESURRECTED_H
#define MESSAGE_CHARACTER_HAS_BEEN_RESURRECTED_H

#include "BlackBoard.h"

/**
Message to notify that the specified character was dead, but now is in an
alive state again.
*/
class MessageCharacterHasBeenResurrected : public Message
{
public:
	MessageCharacterHasBeenResurrected()
	: Message("MessageCharacterHasBeenResurrected")
	{}
};

#endif
