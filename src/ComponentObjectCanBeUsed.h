#ifndef COMPONENT_OBJECT_CAN_BE_USED_H
#define COMPONENT_OBJECT_CAN_BE_USED_H

#include "PropertyBag.h"
#include "Component.h"

/** Object can be approached and then used by some actor */
class ComponentObjectCanBeUsed : public Component
{
public:
	virtual string getTypeString() const { return "ObjectCanBeUsed"; }

	ComponentObjectCanBeUsed(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds) { /* Does nothing */ }

	/** Draws any visual component represented here */
	virtual void draw() const { /* Does nothing */ }

private:
	void handleMessageRequestTriggerAction(Message &message);
	void handleMessageClientApproachesObject(Message &message);
	void handleMessageClientRecedesFromObject(Message &message);

private:
	map<OBJECT_ID, bool> playerNear;
};

#endif
