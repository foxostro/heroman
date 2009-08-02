#include "stdafx.h"
#include "MessageExitMap.h"
#include "MessagePlaySound.h"
#include "ComponentExitMapOnUse.h"

ComponentExitMapOnUse::ComponentExitMapOnUse(BlackBoardUID uid,
											 BlackBoard *blackBoard)
: Component(uid, blackBoard),
  world(0)
{
	registerMessageHandler("MessageClientUsesObject", bind(&ComponentExitMapOnUse::handleMessageClientUsesObject, this, _1));
	registerMessageHandler("MessageSetWorld",         bind(&ComponentExitMapOnUse::handleMessageSetWorld, this, _1));
	resetMembers();
}

void ComponentExitMapOnUse::resetMembers()
{
	nextMap = FileName("data/maps/nil");
	sfxOnFail = FileName("data/sounds/default.wav");
	requiresCoins = false;
}

void ComponentExitMapOnUse::load(const PropertyBag &data)
{
	resetMembers();
	nextMap = data.getFileName("nextMap");
	sfxOnFail = data.getFileName("sfxOnFailed");
	requiresCoins = data.getBool("requiresCoins");
}

void ComponentExitMapOnUse::update(float milliseconds)
{
	// Do Nothing
}

void ComponentExitMapOnUse::handleMessageSetWorld(Message &message)
{
	world = message.getField<World*const>("world");
}

void ComponentExitMapOnUse::handleMessageClientUsesObject(Message &message)
{
	BlackBoard &bb = getParentBlackBoard();

	if(coinRequirementSatisfied())
	{
		bb.relayGlobalMessage(MessageExitMap(nextMap));
	}
	else
	{
		bb.relayGlobalMessage(MessagePlaySound(sfxOnFail));
	}
}

bool ComponentExitMapOnUse::coinRequirementSatisfied() const
{
	ASSERT(world, "Null pointer: world");

	return (!requiresCoins) ||
		   (requiresCoins && world->getNumberOfCoinsRemaining()==0);
}