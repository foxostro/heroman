#include "stdafx.h"
#include "World.h"
#include "Actor.h"
#include "MessageClientApproachesObject.h"
#include "MessageClientRecedesFromObject.h"
#include "MessageHighlightModel.h"
#include "ComponentHighlightOnApproach.h"

ComponentHighlightOnApproach::
ComponentHighlightOnApproach(BlackBoardUID uid, BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageClientApproachesObject",  bind(&ComponentHighlightOnApproach::handleMessageClientApproachesObject, this, _1));
	registerMessageHandler("MessageClientRecedesFromObject", bind(&ComponentHighlightOnApproach::handleMessageClientRecedesFromObject, this, _1));
	resetMembers();
}

void ComponentHighlightOnApproach::handleMessageClientApproachesObject(Message &)
{
	MessageEnableModelHighlight m(HighlightOutline, 60000.0f);
	getParentBlackBoard().relayMessage(m);
}

void ComponentHighlightOnApproach::handleMessageClientRecedesFromObject(Message &)
{
	getParentBlackBoard().relayMessage(MessageDisableModelHighlight());
}
