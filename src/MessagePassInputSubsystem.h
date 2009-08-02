#ifndef MESSAGE_PASS_INPUT_SUBSYSTEM_H
#define MESSAGE_PASS_INPUT_SUBSYSTEM_H

#include "BlackBoard.h"

class SDLinput; // prototype

/** Message that passes a handle to the input subsystem to the recipient */
class MessagePassInputSubsystem : public Message
{
public:
	MessagePassInputSubsystem(SDLinput*const input)
	: Message("MessagePassInputSubsystem")
	{
		addField("input", input);
	}
};

#endif
