#ifndef MESSAGE_CHANGE_SCORE_H
#define MESSAGE_CHANGE_SCORE_H

#include "BlackBoard.h"
#include "Actor.h"


/** 
Message to request that a score be changed. The value specifies the
amount by which to change the score. The operation is the mathematical
operation performed with the score and that value {+,-,*,/}. The
ignoreModifiers toggle sets whether the current multiplier of the score
(and possibly other factors) should be ignored during the update.
*/
class MessageChangeScore : public Message
{
public:
	MessageChangeScore(int value, char operation, bool ignoreModifiers)
	: Message("MessageChangeScore")
	{
		addField("value", value);
		addField("operation", operation);
		addField("ignoreModifiers", ignoreModifiers);
	}
};

#endif
