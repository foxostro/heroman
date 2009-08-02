#ifndef MESSAGE_SET_PLAYER_NUMBER_H
#define MESSAGE_SET_PLAYER_NUMBER_H

#include "BlackBoard.h"

/** Message to request that the player's number be set/changed */
class MessageSetPlayerNumber : public Message
{
public:
	MessageSetPlayerNumber(const int playerNumber)
	: Message("MessageSetPlayerNumber")
	{
		addField("playerNumber", playerNumber);
	}
};

#endif
