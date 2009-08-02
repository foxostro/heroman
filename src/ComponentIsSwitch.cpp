#include "stdafx.h"
#include "color.h"
#include "World.h"
#include "Actor.h"
#include "MessageSwitchWasToggled.h"
#include "ComponentIsSwitch.h"

ComponentIsSwitch::ComponentIsSwitch(BlackBoardUID uid,
									 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageClientUsesObject", bind(&ComponentIsSwitch::handleMessageClientUsesObject, this, _1));
}

void ComponentIsSwitch::handleMessageClientUsesObject(Message &message)
{
	OBJECT_ID requesterID = message.getField<OBJECT_ID>("requesterID");
	getParentBlackBoard().relayGlobalMessage(MessageSwitchWasToggled(categoryID, requesterID));
}

void ComponentIsSwitch::resetMembers()
{
	categoryID = 0;
}

void ComponentIsSwitch::load(const PropertyBag &data)
{
	resetMembers();
	categoryID = data.getInt("categoryID");
}

void ComponentIsSwitch::update(float milliseconds)
{
	// Do Nothing
}
