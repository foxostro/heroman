#ifndef MESSAGE_DECLARE_GAME_OVER_H
#define MESSAGE_DECLARE_GAME_OVER_H

#include "BlackBoard.h"

/** Message to declare GAME OVER and that the players have lost the game */
class MessageDeclareGameOver : public Message
{
public:
	MessageDeclareGameOver()
	: Message("MessageDeclareGameOver")
	{}
};

#endif
