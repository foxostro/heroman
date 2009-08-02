#include "stdafx.h"
#include "World.h"
#include "SDLinput.h"
#include "MessageSetModel.h"
#include "ComponentModelSetOnPlayerNumber.h"

ComponentModelSetOnPlayerNumber::ComponentModelSetOnPlayerNumber(BlackBoardUID _uid, BlackBoard *_blackBoard)
: Component(_uid, _blackBoard)
{
	registerMessageHandler("MessageSetPlayerNumber", bind(&ComponentModelSetOnPlayerNumber::handleMessageSetPlayerNumber, this, _1));
}

void ComponentModelSetOnPlayerNumber::handleMessageSetPlayerNumber(Message &message)
{
	const int NUM_MODELS = 5;
	const int playerNumber = message.getField<int>("playerNumber");

	FileName playerModels[NUM_MODELS] =
	{
		FileName("data/models/hero-green/hero-green.md3xml"),
		FileName("data/models/hero-red/hero-red.md3xml"),
		FileName("data/models/hero-blue/hero-blue.md3xml"),
		FileName("data/models/hero-yellow/hero-yellow.md3xml"),
		FileName("data/models/theryn/theryn.md3xml")
	};

	getParentBlackBoard().relayMessage(MessageRequestSetModel(playerModels[playerNumber % NUM_MODELS]));
}
