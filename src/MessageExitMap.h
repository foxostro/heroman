#ifndef MESSAGE_EXIT_MAP_H
#define MESSAGE_EXIT_MAP_H

#include "BlackBoard.h"

/** Message to request that the map be exited and the next map be loaded */
class MessageExitMap : public Message
{
public:
	MessageExitMap(const FileName &nextMap)
	: Message("MessageExitMap")
	{
		addField("nextMap", nextMap);
	}
};

#endif
