#include "stdafx.h"
#include "MessagePlaySound.h"
#include "ComponentPlaySoundOnUse.h"

ComponentPlaySoundOnUse::ComponentPlaySoundOnUse(BlackBoardUID uid,
												 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageClientUsesObject", bind(&ComponentPlaySoundOnUse::handleMessageClientUsesObject, this, _1));
}

void ComponentPlaySoundOnUse::resetMembers()
{
	sound = FileName("");
}

void ComponentPlaySoundOnUse::load(const PropertyBag &data)
{
	resetMembers();
	sound = data.getFileName("sound");
}

void ComponentPlaySoundOnUse::update(float milliseconds)
{
	// Do Nothing
}

void ComponentPlaySoundOnUse::handleMessageClientUsesObject(Message &message)
{
	getParentBlackBoard().relayGlobalMessage(MessagePlaySound(sound));
}
