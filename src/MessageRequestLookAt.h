#ifndef MESSAGE_REQUEST_LOOKAT_H
#define MESSAGE_REQUEST_LOOKAT_H

#include "BlackBoard.h"

/**
Request that the object be oriented to stand upward in the XY plane and face
some given point at the object's elevation in the XY plane.
*/
class MessageRequestLookAt : public Message
{
public:
	MessageRequestLookAt(const vec3 &lookDirection)
		: Message("MessageRequestLookAt")
	{
		addField("lookDirection", lookDirection);
	}
};

/** Request to set the object's orientation */
class MessageRequestSetOrientation : public Message
{
public:
	MessageRequestSetOrientation(const mat3 &orientation)
	: Message("MessageRequestSetOrientation")
	{
		addField("orientation", orientation);
	}
};

#endif
