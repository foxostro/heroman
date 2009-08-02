#include "stdafx.h"
#include "World.h"
#include "ComponentExplodeOnDeath.h"

ComponentExplodeOnDeath::ComponentExplodeOnDeath(BlackBoardUID uid,
												 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageCharacterHasDied",      bind(&ComponentExplodeOnDeath::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageOrientationHasBeenSet", bind(&ComponentExplodeOnDeath::handleMessageOrientationHasBeenSet, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",    bind(&ComponentExplodeOnDeath::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageSetWorld",              bind(&ComponentExplodeOnDeath::handleMessageSetWorld, this, _1));
}

void ComponentExplodeOnDeath::resetMembers()
{
	baseDamage = 0;
	soundFileName = FileName("(nil)");
	particlesFileName = FileName("(nil)");
}

void ComponentExplodeOnDeath::load(const PropertyBag &data)
{
	resetMembers();

	baseDamage = data.getInt("baseDamage");
	soundFileName = data.getFileName("soundFileName");
	particlesFileName = data.getFileName("particlesFileName");
}

void ComponentExplodeOnDeath::handleMessageSetWorld(Message &message)
{
	world = message.getField<World*const>("world");
}

void ComponentExplodeOnDeath::handleMessageCharacterHasDied(Message &message)
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
}

void ComponentExplodeOnDeath::handleMessageOrientationHasBeenSet(Message &m)
{
	lastReportedOrientation = m.getField<mat3>("orientation");
}

void ComponentExplodeOnDeath::handleMessagePositionHasBeenSet(Message &m)
{
	lastReportedPosition = m.getField<vec3>("position");
}
