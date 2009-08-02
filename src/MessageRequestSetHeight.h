#ifndef MESSAGE_REQUEST_SET_HEIGHT_H
#define MESSAGE_REQUEST_SET_HEIGHT_H

#include "BlackBoard.h"

/** Message to request that the object's height be set to the given value */
class MessageRequestSetHeight : public Message
{
public:
	MessageRequestSetHeight(const float height)
		: Message("MessageRequestSetHeight")
	{
		addField("height", height);
	}
};

#endif
