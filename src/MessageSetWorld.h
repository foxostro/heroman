#ifndef MESSAGE_SET_WORLD_H
#define MESSAGE_SET_WORLD_H

#include "BlackBoard.h"

class World; // prototype

/** Message to declare the world that the object is contained within */
class MessageSetWorld : public Message
{
public:
	MessageSetWorld(World*const world)
	: Message("MessageSetWorld")
	{
		addField("world", world);
	}
};

#endif
