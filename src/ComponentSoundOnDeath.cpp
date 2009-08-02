#include "stdafx.h"
#include "MessagePlaySound.h"
#include "ComponentSoundOnDeath.h"

ComponentSoundOnDeath::ComponentSoundOnDeath(BlackBoardUID uid, 
                                             BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageCharacterHasDied", bind(&ComponentSoundOnDeath::handleMessageCharacterHasDied, this, _1));
	resetMembers();
}

void ComponentSoundOnDeath::load(const PropertyBag &data)
{
	resetMembers();
	deathSounds = data.getListBag<FileName>("deathSounds");
}

void ComponentSoundOnDeath::resetMembers()
{
	deathSounds.clear();
}

void ComponentSoundOnDeath::handleMessageCharacterHasDied(Message &message)
{
	if(!deathSounds.empty())
	{
		FileName f = deathSounds.getRandom();
		MessagePlaySound m(f);
		getParentBlackBoard().relayGlobalMessage(m);
	}
}
