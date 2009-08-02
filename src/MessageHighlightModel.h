#ifndef MESSAGE_HIGHLIGHT_MODEL_H
#define MESSAGE_HIGHLIGHT_MODEL_H

#include "BlackBoard.h"
#include "color.h"

enum HighlightMode
{
	HighlightDisable,
	HighlightOutline,
	HighlightFlash
};

/** Message to request that the model be highlighted */
class MessageEnableModelHighlight : public Message
{
public:
	MessageEnableModelHighlight(HighlightMode mode,
	                            float time,
								color colora = yellow,
								color colorb = white)
	: Message("MessageEnableModelHighlight")
	{
		addField("mode", mode);
		addField("time", time);
		addField("colora", colora);
		addField("colorb", colorb);
	}
};

/** Message to request that the modelnot be highlighted */
class MessageDisableModelHighlight : public Message
{
public:
	MessageDisableModelHighlight()
	: Message("MessageDisableModelHighlight")
	{ /* Do Nothing */ }
};

#endif
