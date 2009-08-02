#ifndef MESSAGE_CLIENT_APPROACHES_OBJECT_H
#define MESSAGE_CLIENT_APPROACHES_OBJECT_H

#include "BlackBoard.h"

/** Message to declare that a client has closely approached the object */
class MessageClientApproachesObject : public Message
{
public:
	MessageClientApproachesObject(OBJECT_ID id)
		: Message("MessageClientApproachesObject")
	{
		addField("id", id);
	}
};

#endif
