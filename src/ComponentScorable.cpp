#include "stdafx.h"
#include "MessageChangeScore.h"
#include "MessageCharacterHasDied.h"
#include "MessageSetWorld.h"
#include "MessageClientUsesObject.h"
#include "ComponentScorable.h"

/** Constructor */
ComponentScorable::ComponentScorable(BlackBoardUID uid,
									 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageCharacterHasDied",			bind(&ComponentScorable::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageSetWorld",					bind(&ComponentScorable::handleMessageSetWorld, this, _1));
	registerMessageHandler("MessageClientUsesObject",			bind(&ComponentScorable::handleMessageClientUsesObject, this, _1));
	registerMessageHandler("MessageCharacterReceivesDamage",	bind(&ComponentScorable::handleMessageCharacterReceivesDamage, this, _1));
}

/** Destructor */
ComponentScorable::~ComponentScorable()
{
	// Do Nothing
}

void ComponentScorable::resetMembers()
{
	behaviors.clear();
	scoreMultiplier = 0.0;
}

void ComponentScorable::load(const PropertyBag &data)
{
	resetMembers();

	//load necessary data
	string behaviorString = data.getString("scoreBehaviors");

	//get scoring behaviors and associated values
	if (behaviorString.find("OnDeath") != string::npos)
		behaviors[OnDeath]  = data.getInt("scoreOnDeath");
	if (behaviorString.find("OnPickup") != string::npos)
		behaviors[OnPickup] = data.getInt("scoreOnPickup");
	if (behaviorString.find("OnDamage") != string::npos)
		behaviors[OnDamage] = data.getInt("scoreOnDamage");
}

void ComponentScorable::draw() const
{
	//does nothing
}

void ComponentScorable::update(float milliseconds)
{

}

/** Callback handler for MessageCharacterHasDied. */
void ComponentScorable::handleMessageCharacterHasDied(Message &m)
{
	if (behaviors.count(OnDeath) > 0 )
		world->recvMessage(MessageChangeScore(behaviors[OnDeath], '+', false));
}

/** Callback handler for MessageSetWorld. */
void ComponentScorable::handleMessageSetWorld(Message &m)
{
	world = m.getField<World *>("world");
}

/** Callback handler for MessageClientUsesObject. */
void ComponentScorable::handleMessageClientUsesObject(Message &m)
{
	if (behaviors.count(OnPickup) > 0)
		world->recvMessage(MessageChangeScore(behaviors[OnPickup], '+', false));
}

/** Callback handler for MessageCharacterReceievesDamage. */
void ComponentScorable::handleMessageCharacterReceivesDamage(Message &m)
{
	string myname = getActorPtr()->actorName;
	bool b = (myname == "monster_spawn");

	int f = behaviors[OnDamage];
	char buf[50];
	sprintf(buf, "f = %d", f);
	PrintStringToLog(buf);

	if (behaviors.count(OnDamage) > 0)
		world->recvMessage(MessageChangeScore(behaviors[OnDamage], '+', false));
}
