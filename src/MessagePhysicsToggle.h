#ifndef MESSAGE_PHYSICS_TOGGLE_H
#define MESSAGE_PHYSICS_TOGGLE_H

#include "BlackBoard.h"
#include "Actor.h"

class MessagePhysicsDisable : public Message
{
public:
	MessagePhysicsDisable()
	: Message("MessagePhysicsDisable")
	{}
};

class MessagePhysicsEnable : public Message
{
public:
	MessagePhysicsEnable()
	: Message("MessagePhysicsEnable")
	{}
};

#endif
