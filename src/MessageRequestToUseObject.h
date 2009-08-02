#ifndef MESSAGE_REQUEST_TO_USE_OBJECT_H
#define MESSAGE_REQUEST_TO_USE_OBJECT_H

#include "BlackBoard.h"

/** Message to request to activate and "use" some object */
class MessageRequestToUseObject : public Message
{
public:
	MessageRequestToUseObject(OBJECT_ID requesterID)
		: Message("MessageRequestTriggerAction")
	{
		addField("requesterID", requesterID);
	}
};

#endif
