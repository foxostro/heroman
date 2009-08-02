#ifndef MESSAGE_CLIENT_USES_OBJECT_H
#define MESSAGE_CLIENT_USES_OBJECT_H

#include "BlackBoard.h"

/**
Message to declare how a client has requested to use the object.
Some validation has been done by the message sender, but the object ID has
also been sent to further validate the request.
*/
class MessageClientUsesObject : public Message
{
public:
	MessageClientUsesObject(OBJECT_ID requesterID)
		: Message("MessageClientUsesObject")
	{
		addField("requesterID", requesterID);
	}
};

#endif
