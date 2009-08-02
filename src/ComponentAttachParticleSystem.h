#ifndef COMPONENT_ATTACH_PARRTICLE_SYSTEM_H
#define COMPONENT_ATTACH_PARRTICLE_SYSTEM_H

#include "PropertyBag.h"
#include "World.h"
#include "Component.h"

/** Attaches a particle system to the object's position */
class ComponentAttachParticleSystem : public Component
{
public:
	virtual string getTypeString() const { return "AttachParticleSystem"; }

	ComponentAttachParticleSystem(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Updates the object */
	virtual void update(float milliseconds);

private:
	void handleMessageOrientationHasBeenSet(Message &message);
	void handleMessagePositionHasBeenSet(Message &message);
	void handleMessageSetWorld(Message &message);
	void handleMessageZombifyActor(Message &message);

private:
	vec3 lastReportedPosition;
	mat3 lastReportedOrientation;
	World *world;
	ParticleEngine::handle handle;
	bool active;
};

#endif
