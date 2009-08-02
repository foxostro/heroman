#ifndef MESSAGE_PHYSICS_COLLISION_H
#define MESSAGE_PHYSICS_COLLISION_H

#include "BlackBoard.h"

/** Message indicates collision between two collision objects */
class MessagePhysicsCollision : public Message
{
public:
	MessagePhysicsCollision(dGeomID o1, dGeomID o2, dContact contact)
	: Message("MessagePhysicsCollision")
	{
		addField("o1", o1);
		addField("o2", o2);
		addField("contact", contact);
	}
};

#endif
