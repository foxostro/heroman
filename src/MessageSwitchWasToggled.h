#ifndef MESSAGE_SWITCH_HAS_BEEN_TOGGLED_H
#define MESSAGE_SWITCH_HAS_BEEN_TOGGLED_H

#include "BlackBoard.h"

/** Message to notify that some switch in the map has been toggled */
class MessageSwitchWasToggled : public Message
{
public:
	MessageSwitchWasToggled(int categoryID, OBJECT_ID requesterID)
		: Message("MessageSwitchWasToggled")
	{
		addField("categoryID", categoryID);
		addField("requesterID", requesterID);
	}
};

#endif
