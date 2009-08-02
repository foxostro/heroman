#include "stdafx.h"
#include "World.h"
#include "MessageZombifyActor.h"
#include "ComponentExplodeAfterTimeout.h"

ComponentExplodeAfterTimeout::ComponentExplodeAfterTimeout(BlackBoardUID uid,
												 BlackBoard *blackBoard)
												 : Component(uid, blackBoard)
{
	registerMessageHandler("MessageOrientationHasBeenSet", bind(&ComponentExplodeAfterTimeout::handleMessageOrientationHasBeenSet, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",    bind(&ComponentExplodeAfterTimeout::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageSetWorld",              bind(&ComponentExplodeAfterTimeout::handleMessageSetWorld, this, _1));
}

void ComponentExplodeAfterTimeout::resetMembers()
{
	timeleft = 0.0f;
	baseDamage = 0;
	soundFileName = FileName("(nil)");
	particlesFileName = FileName("(nil)");
}

void ComponentExplodeAfterTimeout::load(const PropertyBag &data)
{
	resetMembers();

	timeleft = data.getFloat("timeout");
	baseDamage = data.getInt("baseDamage");
	soundFileName = data.getFileName("soundFileName");
	particlesFileName = data.getFileName("particlesFileName");
}

void ComponentExplodeAfterTimeout::handleMessageSetWorld(Message &message)
{
	world = message.getField<World*const>("world");
}

void ComponentExplodeAfterTimeout::handleMessageOrientationHasBeenSet(Message &m)
{
	lastReportedOrientation = m.getField<mat3>("orientation");
}

void ComponentExplodeAfterTimeout::handleMessagePositionHasBeenSet(Message &m)
{
	lastReportedPosition = m.getField<vec3>("position");
}

void ComponentExplodeAfterTimeout::explode()
{
	ASSERT(world, "world has not yet been set");

	const vec2 x = lastReportedOrientation.getAxisX().xy();
	const float rotation = getAngle(x, vec2(1,0));

	world->generateExplosion(getActorPtr()->getUID(),
	                         lastReportedPosition,
	                         rotation,
	                         baseDamage,
	                         soundFileName,
	                         particlesFileName);

	getParentBlackBoard().relayMessage(MessageZombifyActor());
}

void ComponentExplodeAfterTimeout::update( float milliseconds )
{
	if((timeleft -= milliseconds) <= 0.0f)
	{
		explode();
	}
}
