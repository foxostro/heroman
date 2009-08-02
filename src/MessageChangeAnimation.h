#ifndef MESSAGE_CHANGE_ANIMATION_H
#define MESSAGE_CHANGE_ANIMATION_H

#include "BlackBoard.h"

/** Message to request that the object's animation be changed */
class MessageChangeAnimation : public Message
{
public:
	MessageChangeAnimation(const string &animationName)
	: Message("MessageChangeAnimation")
	{
		addField("animationName", animationName);
	}
};

#endif
