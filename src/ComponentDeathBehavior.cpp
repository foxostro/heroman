#include "stdafx.h"
#include "MessagePhysicsToggle.h"
#include "ComponentDeathBehavior.h"

ComponentDeathBehavior::ComponentDeathBehavior(BlackBoardUID uid,
											   BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageCharacterHasBeenResurrected", bind(&ComponentDeathBehavior::handleMessageCharacterHasBeenResurrected, this, _1));
	registerMessageHandler("MessageCharacterHasDied",            bind(&ComponentDeathBehavior::handleMessageCharacterHasDied, this, _1));
}

void ComponentDeathBehavior::resetMembers()
{
	deathBehavior = Corpse;
}

void ComponentDeathBehavior::load(const PropertyBag &data)
{
	resetMembers();
	deathBehavior = deathBehaviorFromString(data.getString("deathBehavior"));
}

DeathBehavior ComponentDeathBehavior::deathBehaviorFromString(const string &_s)
{
	string s = toLowerCase(_s);

	if(s == "corpse")
	{
		return Corpse;
	}
	else if(s == "disappear")
	{
		return Disappear;
	}
	else if(s == "ghost")
	{
		return Ghost;
	}

	FAIL("Invalid death behavior string: " + _s);
	return Corpse;
}

void ComponentDeathBehavior::handleMessageCharacterHasBeenResurrected( Message &message )
{
	getParentBlackBoard().relayMessage(MessagePhysicsEnable());
}

void ComponentDeathBehavior::handleMessageCharacterHasDied(Message &message)
{
	if(getDeathBehavior() != Ghost)
	{
		getParentBlackBoard().relayMessage(MessagePhysicsDisable());
	}
}
