#ifndef COMPONENT_USE_ON_COLLISION_H
#define COMPONENT_USE_ON_COLLISION_H

#include "PropertyBag.h"
#include "Component.h"

/**
Component fires a use message on collision between this actor and any
other actor.
*/
class ComponentUseOnCollision : public Component
{
public:
	virtual string getTypeString() const { return "UseOnCollision"; }

	ComponentUseOnCollision(BlackBoardUID uid, BlackBoard *blackBoard);

	virtual void update(float) {}

	virtual void load(const PropertyBag &data);

	virtual void resetMembers();

private:
	void handleMessagePhysicsCollision(Message &_message);
	void handleMessageSetWorld(Message &m);
	bool isPlayer(Actor *actor) const;

private:
	World *world;
	bool onlyPlayers;
};

#endif
