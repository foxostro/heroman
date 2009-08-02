#ifndef MESSAGE_PASS_FONT_H
#define MESSAGE_PASS_FONT_H

#include "BlackBoard.h"
#include "Actor.h"

/** Message to pass a font to an actor for use in rendering its GUI */
class MessagePassFont : public Message
{
public:
	MessagePassFont(shared_ptr<Font> font)
	: Message("MessagePassFont")
	{
		addField("font", font);
	}
};

#endif
