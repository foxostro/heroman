#ifndef COMPONENT_HIGHLIGHT_ON_APPROACH_H
#define COMPONENT_HIGHLIGHT_ON_APPROACH_H

#include "PropertyBag.h"
#include "Component.h"

/** Object can be approached and then used by some actor */
class ComponentHighlightOnApproach : public Component
{
public:
	virtual string getTypeString() const { return "HighlightOnApproach"; }

	ComponentHighlightOnApproach(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers() { /* Do Nothing */ }

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data) { /* Do Nothing */ }

	/**
	Updates component each tick
	@param milliseconds Time since the last tick
	*/
	virtual void update(float milliseconds) { /* Does nothing */ }

	/** Draws any visual component represented here */
	virtual void draw() const { /* Does nothing */ }

private:
	void handleMessageClientApproachesObject(Message &message);
	void handleMessageClientRecedesFromObject(Message &message);
};

#endif
