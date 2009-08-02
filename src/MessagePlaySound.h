#ifndef MESSAGE_PLAY_SOUND_H
#define MESSAGE_PLAY_SOUND_H

#include "BlackBoard.h"

/** Message to request that a sound be played */
class MessagePlaySound : public Message
{
public:
	MessagePlaySound(const FileName &sound)
	: Message("MessagePlaySound")
	{
		addField("sound", sound);
	}
};

#endif
