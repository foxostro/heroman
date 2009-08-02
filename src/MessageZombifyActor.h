#ifndef MESSAGE_ZOMBIFY_ACTOR_H
#define MESSAGE_ZOMBIFY_ACTOR_H

#include "BlackBoard.h"

/** 
Message to request that the object become a zombie and be removed during the
next garbage collection operation
*/
class MessageZombifyActor : public Message
{
public:
	MessageZombifyActor()
	: Message("MessageZombifyActor")
	{}
};

#endif
