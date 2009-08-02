#ifndef COMPONENT_DAMAGE_ON_COLLISION_H
#define COMPONENT_DAMAGE_ON_COLLISION_H

#include "PropertyBag.h"
#include "Component.h"

/**
Component fires a character damage message to any actors that this actor
collides with.
*/
class ComponentDamageOnCollision : public Component
{
public:
	virtual string getTypeString() const { return "DamageOnCollision"; }

	ComponentDamageOnCollision(BlackBoardUID uid, BlackBoard *blackBoard);

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
	int damage;
};

#endif
