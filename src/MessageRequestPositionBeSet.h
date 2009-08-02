#ifndef MESSAGE_REQUEST_POSITION_BE_SET_H
#define MESSAGE_REQUEST_POSITION_BE_SET_H

#include "BlackBoard.h"

/** Request that the object's position be moved to the specified point */
class MessageRequestPositionBeSet : public Message
{
public:
	MessageRequestPositionBeSet(const vec3 &position)
	: Message("MessageRequestPositionBeSet")
	{
		addField("position", position);
	}
};

/**
Sent by the world to declare an object's initial position and velocity
*/
class MessageDeclareInitialPosition : public Message
{
public:
	MessageDeclareInitialPosition(const vec3 &position,
	                              const vec3 velocity = vec3(0,0,0))
	: Message("MessageDeclareInitialPosition")
	{
		addField("position", position);
		addField("velocity", velocity);
	}
};

#endif
