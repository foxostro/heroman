#ifndef MESSAGE_COLLISION_H
#define MESSAGE_COLLISION_H

#include "BlackBoard.h"

/**
Collision detection message.
The information used to make the collision dsetermination is attached.
*/
class MessageCollision : public Message
{
public:
	MessageCollision(const float &distance,
	                 const float &ourRadius,
	                 const float &theirRadius,
					 const vec2 &ourPosition,
					 const vec2 &theirPosition)
	: Message("MessageCollision")
	{
		addField("distance", distance);
		addField("ourRadius", ourRadius);
		addField("theirRadius", theirRadius);
		addField("ourPosition", ourPosition);
		addField("theirPosition", theirPosition);
	}
};

#endif
