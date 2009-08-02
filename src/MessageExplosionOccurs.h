#ifndef MESSAGE_EXPLOSION_OCCURS_H
#define MESSAGE_EXPLOSION_OCCURS_H

#include "BlackBoard.h"

/** Notofy that an explosion occurred somewhere nearby */
class MessageExplosionOccurs : public Message
{
public:
	MessageExplosionOccurs(const vec3 &position,
	                       int baseDamage,
						   OBJECT_ID actor)
		: Message("MessageExplosionOccurs")
	{
		addField("position", position);
		addField("baseDamage", baseDamage);
		addField("actor", actor);
	}
};

#endif
