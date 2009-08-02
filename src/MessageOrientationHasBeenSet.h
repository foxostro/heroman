#ifndef MESSAGE_ORIENTATION_HAS_BEEN_SET_H
#define MESSAGE_ORIENTATION_HAS_BEEN_SET_H

#include "BlackBoard.h"

/**
Message to notify that the object's orientation has been set for the frame.
The orientation may not have necessarily changed since the last update.
*/
class MessageOrientationHasBeenSet : public Message
{
public:
	MessageOrientationHasBeenSet(const mat3 &orientation)
	: Message("MessageOrientationHasBeenSet")
	{
		addField("orientation", orientation);
	}
};

#endif
