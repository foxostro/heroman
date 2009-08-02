#ifndef COMPONENT_MODEL_SET_ON_PLAYER_NUMBER_H
#define COMPONENT_MODEL_SET_ON_PLAYER_NUMBER_H

#include "PropertyBag.h"
#include "Component.h"

/**
Changes the object model when it receives a player number for this object.
Really only applicable to Player Character objects...
*/
class ComponentModelSetOnPlayerNumber : public Component
{
public:
	virtual string getTypeString() const { return "ModelSetOnPlayerNumber"; }

	ComponentModelSetOnPlayerNumber(BlackBoardUID _uid, BlackBoard *_blackBoard);

	/** Does nothing */
	virtual void load(const PropertyBag &data) {}

	/** Does nothing */
	virtual void update(float) {}

	/** Does nothing */
	virtual void resetMembers() {}

private:
	/** Sets the player number */
	void handleMessageSetPlayerNumber(Message &_message);
};

#endif
