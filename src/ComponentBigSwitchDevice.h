#ifndef COMPONENT_BIG_SWITCH_DEVICE_H
#define COMPONENT_BIG_SWITCH_DEVICE_H

#include "PropertyBag.h"
#include "Component.h"

class World;

/** Renders the object as a wireframe switch device */
class ComponentBigSwitchDevice : public Component
{
public:
	virtual string getTypeString() const { return "BigSwitchDevice"; }

	ComponentBigSwitchDevice(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Updates the object */
	virtual void update(float milliseconds);

private:
	enum SwitchState
	{
		STATE_A,
		STATE_B,
		TRANSITION_BA,
		TRANSITION_AB
	};

private:
	void update_PositionA();
	void update_PositionB();
	void update_TransitionToA();
	void update_TransitionToB();

	void enterState(SwitchState newState);

	void handleMessagePositionHasBeenSet(Message &message);
	void handleMessageOrientationHasBeenSet(Message &message);
	void handleMessageRequestSetHeight(Message &message);
	void handleMessageClientUsesObject(Message &message);
	void handleMessageEnableModelHighlight(Message &message);
	void handleMessageDisableModelHighlight(Message &message);

	void drawLever() const;
	color getLeverColor() const;
	void drawCircle(float radius, float z) const;
	void drawBase(const int numIter,
	              const float radius,
	              const float eps) const;

private:
	vec3 lastReportedPosition;
	mat3 lastReportedOrientation;
	float lastReportedHeight;

	float positionA;
	float positionB;
	float angle;
	float timeInTransition;
	float timeToTransition;
	SwitchState state;
	map<SwitchState, function<void ()> > stateHandler;
	int categoryID;
	bool highlight;
	float highlightIntensity;
	float hightlightIntensityRate;
	color colora;
	color colorb;
};

#endif
