#include "stdafx.h"
#include "ParticleSystem.h"
#include "ComponentPhysics.h"
#include "ComponentAttachParticleSystem.h"

ComponentAttachParticleSystem::
ComponentAttachParticleSystem(BlackBoardUID uid, BlackBoard *blackBoard)
: Component(uid, blackBoard),
  world(0),
  active(true)
{
	registerMessageHandler("MessageOrientationHasBeenSet", bind(&ComponentAttachParticleSystem::handleMessageOrientationHasBeenSet, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",    bind(&ComponentAttachParticleSystem::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageSetWorld",              bind(&ComponentAttachParticleSystem::handleMessageSetWorld, this, _1));
	registerMessageHandler("MessageZombifyActor",          bind(&ComponentAttachParticleSystem::handleMessageZombifyActor, this, _1));

	resetMembers();
}

void ComponentAttachParticleSystem::resetMembers()
{
	lastReportedPosition.zero();
	lastReportedOrientation.identity();
}

void ComponentAttachParticleSystem::update(float milliseconds)
{
	if(!active) return;

	ParticleSystem *system = *handle;

	if(system)
	{
		const vec2 x = lastReportedOrientation.getAxisX().xy();
		const float rotation = getAngle(x, vec2(1,0));
		system->setRotation(rotation);
		system->setPosition(lastReportedPosition);
	}
}

void ComponentAttachParticleSystem::load(const PropertyBag &data)
{
	ASSERT(world, "world has not yet been set");

	const vec3 position = vec3(0,0,0);
	const float rotation = 0.0f;
	const FileName fileName = data.getFileName("fileName");

	handle = world->particleEngine.add(fileName,
	                                   position,
							           rotation,
							           world->getTextureFactory());
}

void ComponentAttachParticleSystem::handleMessageOrientationHasBeenSet(Message &message)
{
	lastReportedOrientation = message.getField<mat3>("orientation");
}

void ComponentAttachParticleSystem::handleMessagePositionHasBeenSet(Message &message)
{
	lastReportedPosition = message.getField<vec3>("position");
}

void ComponentAttachParticleSystem::handleMessageSetWorld(Message &message)
{
	world = message.getField<World*const>("world");
}

void ComponentAttachParticleSystem::handleMessageZombifyActor(Message &m)
{
	ASSERT(world, "world has not yet been set");
	world->particleEngine.remove(handle);
	active=false;
}
