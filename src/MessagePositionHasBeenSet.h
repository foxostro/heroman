#ifndef MESSAGE_POSITION_HAS_BEEN_SET_H
#define MESSAGE_POSITION_HAS_BEEN_SET_H

#include "BlackBoard.h"

/**
Message to notify that the object's position has been set for the frame.
(The position may not have necessarily changed since the last update.)
*/
class MessagePositionHasBeenSet : public Message
{
public:
	MessagePositionHasBeenSet(const vec3 &position)
	: Message("MessagePositionHasBeenSet")
	{
		addField("position", position);
	}
};

#endif
