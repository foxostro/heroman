#ifndef MESSAGE_REQUEST_APPLY_FORCE_H
#define MESSAGE_REQUEST_APPLY_FORCE_H

#include "BlackBoard.h"

/** Request that some force be applied to the object */
class MessageRequestApplyForce : public Message
{
public:
	MessageRequestApplyForce(const vec3 &force)
	: Message("MessageRequestApplyForce")
	{
		addField("force", force);
	}
};

#endif
