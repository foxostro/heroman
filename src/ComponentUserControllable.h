#ifndef COMPONENT_USER_CONTROLLABLE_H
#define COMPONENT_USER_CONTROLLABLE_H

#include "PropertyBag.h"
#include "Component.h"
#include "SDLinput.h"

/**
The object can be controlled by the user.  This may be aggregated with other
components to make up a Player Character object.

And here are some tasty definitions.  These terms are often used
interchangeably, so I'm trying to pin down their exact meaning for the
purposes of this game:
    User   - Person physically sitting in front of the game
    Player - User's avatar, also known as the "Player Character"
*/
class ComponentUserControllable : public Component
{
public:
	virtual string getTypeString() const { return "UserControllable"; }

	virtual ~ComponentUserControllable();

	ComponentUserControllable(BlackBoardUID _uid, BlackBoard *_blackBoard);

	/** Does nothing */
	virtual void load(const PropertyBag &data);

	/** Updates component each tick */
	virtual void update(float milliseconds);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Gets the player number */
	int getPlayerNumber() const;

private:
	/** Sets the player number */
	void handleMessageSetPlayerNumber(Message &_message);

	/**
	Registers user input callbacks.
	Requires that the player numner already be set!
	*/
	void handleMessagePassInputSubsystem(Message &_message);

	/** Called when the actor is about to be removed from the game world */
	void handleMessageZombifyActor(Message &_message);

	/** Actor position has been set */
	void handleMessagePositionHasBeenSet(Message &message);

	/** Sets up handlers (callbacks) for user input events */
	void setupInputHandlers();

	void releaseInputHandlers();

	void onKeyPressUse();
	void onKeyPressAttack();
	void onKeyPressChargeUp();
	void onKeyPressSuicide();

	void onKeyDownFwd();
	void onKeyDownRev();
	void onKeyDownLeft();
	void onKeyDownRight();

	void onKeyDownNorth();
	void onKeyDownSouth();
	void onKeyDownEast();
	void onKeyDownWest();

	void onJoystickAxisX(int value);
	void onJoystickAxisY(int value);

	void onJoystickAxisXRot(int value);
	void onJoystickAxisYRot(int value);

	void onMouseMove(ivec2 xy, ivec2 xyrel);
	void onMouseDownLeft(int x, int y) { onKeyPressChargeUp(); }
	void onMouseUpLeft(int x, int y) { onKeyPressAttack(); }
	void onMouseDownRight(int x, int y) { onKeyPressUse(); }

private:
	float mouseSensitivity;
	vec3 lastReportedPosition;
	int playerNumber;
	bool north, south, east, west, w, s, a , d;
	SDLinput *input;
	SDLinput::HANDLE handleGamePadBtnR_down;
	SDLinput::HANDLE handleGamePadBtnR_up;
	SDLinput::HANDLE handleGamePadBtnL_down;
	SDLinput::HANDLE handleGamePadBtnL_up;
	SDLinput::HANDLE handleGamePadBtnY;
	SDLinput::HANDLE handleGamePadAxisX;
	SDLinput::HANDLE handleGamePadAxisY;
	SDLinput::HANDLE handleGamePadAxisXRot;
	SDLinput::HANDLE handleGamePadAxisYRot;
	SDLinput::HANDLE handleKeyboardF;
	SDLinput::HANDLE handleKeyboardE;
	SDLinput::HANDLE handleKeyboardSpace_down;
	SDLinput::HANDLE handleKeyboardSpace_up;
	SDLinput::HANDLE handleKeyboardW;
	SDLinput::HANDLE handleKeyboardS;
	SDLinput::HANDLE handleKeyboardA;
	SDLinput::HANDLE handleKeyboardD;
	SDLinput::HANDLE handleMouseMove;
	SDLinput::HANDLE handleMouseRightClick;
	SDLinput::HANDLE handleMouseLeftDown;
	SDLinput::HANDLE handleMouseLeftUp;
};

#endif
