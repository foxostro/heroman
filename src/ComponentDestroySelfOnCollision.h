#ifndef COMPONENT_DESTROY_SELF_ON_COLLISION_H
#define COMPONENT_DESTROY_SELF_ON_COLLISION_H

#include "PropertyBag.h"
#include "Component.h"

/**
Component ensures that actor destroys itself when a collision with another
actor occurs. (e.g. Projectile such as a missile)
*/
class ComponentDestroySelfOnCollision : public Component
{
public:
	virtual string getTypeString() const { return "DestroySelfOnCollision"; }

	ComponentDestroySelfOnCollision(BlackBoardUID uid, BlackBoard *blackBoard);

	virtual void update(float);

	virtual void load(const PropertyBag &data);

	virtual void resetMembers();

private:
	void handleMessagePhysicsCollision(Message &_message);
	void handleMessageSetWorld(Message &m);

	bool isPlayer(Actor *actor) const;

private:
	World *world;
	bool shouldActivate;
	bool onlyPlayers;
};

#endif
