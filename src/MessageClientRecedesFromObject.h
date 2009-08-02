#ifndef MESSAGE_CLIENT_RECEDES_FROM_OBJECT_H
#define MESSAGE_CLIENT_RECEDES_FROM_OBJECT_H

#include "BlackBoard.h"

/**
Message to declare that a client used to, but no longer,
closely approaches the object.
*/
class MessageClientRecedesFromObject : public Message
{
public:
	MessageClientRecedesFromObject(OBJECT_ID id)
		: Message("MessageClientRecedesFromObject")
	{
		addField("id", id);
	}
};

#endif
