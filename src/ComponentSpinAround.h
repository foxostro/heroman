#ifndef COMPONENT_SPIN_AROUND_H
#define COMPONENT_SPIN_AROUND_H

#include "PropertyBag.h"
#include "Component.h"

class World;

/** Spins the actor around in place */
class ComponentSpinAround : public Component
{
public:
	virtual string getTypeString() const { return "SpinAround"; }

	ComponentSpinAround(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds);

private:
	void handleMessageDeclareInitialPosition(Message &m);

private:
	vec3 initialPosition;
	float facingAngle;
	float rotationSpeed;
	float bounce;
	float bounceSpeed;
	float bounceHeight;
};

#endif
