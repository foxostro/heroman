#include "stdafx.h"
#include "World.h"
#include "Actor.h"
#include "MessageClientApproachesObject.h"
#include "MessageClientRecedesFromObject.h"
#include "MessageClientUsesObject.h"
#include "ComponentObjectCanBeUsed.h"

ComponentObjectCanBeUsed::ComponentObjectCanBeUsed(BlackBoardUID uid,
									               BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageRequestTriggerAction",    bind(&ComponentObjectCanBeUsed::handleMessageRequestTriggerAction, this, _1));
	registerMessageHandler("MessageClientApproachesObject",  bind(&ComponentObjectCanBeUsed::handleMessageClientApproachesObject, this, _1));
	registerMessageHandler("MessageClientRecedesFromObject", bind(&ComponentObjectCanBeUsed::handleMessageClientRecedesFromObject, this, _1));
	resetMembers();
}

void ComponentObjectCanBeUsed::handleMessageClientApproachesObject( Message &message )
{
	playerNear[message.getField<OBJECT_ID>("id")] = true;
}

void ComponentObjectCanBeUsed::handleMessageClientRecedesFromObject( Message &message )
{
	playerNear[message.getField<OBJECT_ID>("id")] = false;
}

void ComponentObjectCanBeUsed::resetMembers()
{
	playerNear.clear();
}

void ComponentObjectCanBeUsed::load( const PropertyBag &data )
{
	resetMembers();
}

void ComponentObjectCanBeUsed::handleMessageRequestTriggerAction( Message &message )
{
	const OBJECT_ID requesterID = message.getField<OBJECT_ID>("requesterID");

	if(playerNear[requesterID])
	{
		getParentBlackBoard().relayMessage(MessageClientUsesObject(requesterID));
	}
}
