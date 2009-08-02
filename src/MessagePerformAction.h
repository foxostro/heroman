#ifndef MESSAGE_PERFORM_ACTION_H
#define MESSAGE_PERFORM_ACTION_H

#include "BlackBoard.h"
#include "Actions.h"

/** Message to request that object perform some action in the game world */
class MessagePerformAction : public Message
{
public:
	MessagePerformAction(Action action)
	: Message("MessagePerformAction")
	{
		addField("action", action);
	}
};

#endif
