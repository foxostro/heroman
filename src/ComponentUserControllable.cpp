#include "stdafx.h"
#include "World.h"
#include "SDLinput.h"
#include "MessagePassInputSubsystem.h"
#include "MessageSetPlayerNumber.h"
#include "MessagePerformAction.h"
#include "MessageRequestToUseObject.h"
#include "MessageRequestCharacterFaceAngle.h"
#include "ComponentUserControllable.h"

ComponentUserControllable::ComponentUserControllable(BlackBoardUID _uid,
									                 BlackBoard *_blackBoard)
: Component(_uid, _blackBoard),
  input(0)
{
	north = south = east = west = false;
	w = s = a = d = false;

	registerMessageHandler("MessagePassInputSubsystem", bind(&ComponentUserControllable::handleMessagePassInputSubsystem, this, _1));
	registerMessageHandler("MessageSetPlayerNumber",    bind(&ComponentUserControllable::handleMessageSetPlayerNumber, this, _1));
	registerMessageHandler("MessageZombifyActor",       bind(&ComponentUserControllable::handleMessageZombifyActor, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet", bind(&ComponentUserControllable::handleMessagePositionHasBeenSet, this, _1));
}

ComponentUserControllable::~ComponentUserControllable()
{
	releaseInputHandlers();
	resetMembers();
}

void ComponentUserControllable::resetMembers()
{
	playerNumber = -1;
	handleGamePadBtnR_down = -1;
	handleGamePadBtnR_up = -1;
	handleGamePadBtnL_down = -1;
	handleGamePadBtnL_up = -1;
	handleGamePadBtnY = -1;
	handleGamePadAxisX = -1;
	handleGamePadAxisY = -1;
	handleGamePadAxisXRot = -1;
	handleGamePadAxisYRot = -1;
	handleKeyboardSpace_down = -1;
	handleKeyboardSpace_up = -1;
	handleKeyboardF = -1;
	handleKeyboardE = -1;
	handleKeyboardW = -1;
	handleKeyboardS = -1;
	handleKeyboardA = -1;
	handleKeyboardD = -1;
	handleMouseMove = -1;
	handleMouseLeftDown = -1;
	handleMouseLeftUp = -1;
	handleMouseRightClick = -1;
	lastReportedPosition.zero();
	mouseSensitivity = 100.0f;
}

void ComponentUserControllable::update(float)
{
	const bool moveKey = north || south || east || west || w || s || a || d;

	Action action;

	     if(north && east) action = StepNorthWest;
	else if(north && west) action = StepNorthEast;
	else if(south && east) action = StepSouthWest;
	else if(south && west) action = StepSouthEast;
	else if(north)         action = StepNorth;
	else if(south)         action = StepSouth;
	else if(east)          action = StepWest;
	else if(west)          action = StepEast;
	else if(w && d)        action = StepForwardRight;
	else if(w && a)        action = StepForwardLeft;
	else if(s && d)        action = StepBackWardRight;
	else if(s && a)        action = StepBackWardLeft;
	else if(w)             action = StepForward;
	else if(s)             action = StepBackWard;
	else if(a)             action = StepLeft;
	else if(d)             action = StepRight;
	else                   action = Stand;

	getParentBlackBoard().relayMessage(MessagePerformAction(action));

	north = south = east = west = false;
	w = s = a = d = false;
}

void ComponentUserControllable::handleMessageZombifyActor(Message &message)
{
	releaseInputHandlers();
	resetMembers();
}

void ComponentUserControllable::handleMessagePassInputSubsystem(Message &message)
{
	input = message.getField<SDLinput*const>("input");

	setupInputHandlers(); // register input event handlers (callbacks)
}

void ComponentUserControllable::handleMessageSetPlayerNumber(Message &message)
{
	playerNumber = message.getField<int>("playerNumber");
}

void ComponentUserControllable::onKeyPressUse()
{
	BlackBoardUID actorUID = getParentBlackBoard().getUID();
	MessageRequestToUseObject message(actorUID);
	getParentBlackBoard().relayGlobalMessage(message);
}

void ComponentUserControllable::onKeyDownFwd()
{
	w = true;
}

void ComponentUserControllable::onKeyDownRev()
{
	s = true;
}

void ComponentUserControllable::onKeyDownLeft()
{
	a = true;
}

void ComponentUserControllable::onKeyDownRight()
{
	d = true;
}

void ComponentUserControllable::onKeyDownNorth()
{
	north = true;
}

void ComponentUserControllable::onKeyDownSouth()
{
	south = true;
}

void ComponentUserControllable::onKeyDownEast()
{
	east = true;
}

void ComponentUserControllable::onKeyDownWest()
{
	west = true;
}

void ComponentUserControllable::onJoystickAxisX(int value)
{
	d = value > 0;
	a = value < 0;
}

void ComponentUserControllable::onJoystickAxisY(int value)
{
	s = value > 0;
	w = value < 0;
}

void ComponentUserControllable::setupInputHandlers()
{
	const int playerNumber = getPlayerNumber();

	ASSERT(input,           "Have not yet received input subsystem!");
	ASSERT(playerNumber>=0, "Have not received updated player number!");

	const int joystickNumber = playerNumber;


	handleGamePadBtnR_down = input->registerJoyButtonPressHandler(joystickNumber, XPAD_BTN_R, bind(&ComponentUserControllable::onKeyPressChargeUp, this));
	handleGamePadBtnR_up = input->registerJoyButtonUpHandler(joystickNumber, XPAD_BTN_R, bind(&ComponentUserControllable::onKeyPressAttack, this));
	handleGamePadBtnL_down = input->registerJoyButtonPressHandler(joystickNumber, XPAD_BTN_L, bind(&ComponentUserControllable::onKeyPressChargeUp, this));
	handleGamePadBtnL_up = input->registerJoyButtonUpHandler(joystickNumber, XPAD_BTN_L, bind(&ComponentUserControllable::onKeyPressAttack, this));
	handleGamePadBtnY = input->registerJoyButtonPressHandler(joystickNumber, XPAD_BTN_Y, bind(&ComponentUserControllable::onKeyPressUse, this));
	handleGamePadAxisX = input->registerJoyAxisMotionHandler(joystickNumber, XPAD_AXIS_X, bind(&ComponentUserControllable::onJoystickAxisX, this, _1));
	handleGamePadAxisY = input->registerJoyAxisMotionHandler(joystickNumber, XPAD_AXIS_Y, bind(&ComponentUserControllable::onJoystickAxisY, this, _1));
	handleGamePadAxisXRot = input->registerJoyAxisMotionHandler(joystickNumber, XPAD_AXIS_XROT, bind(&ComponentUserControllable::onJoystickAxisXRot, this, _1));
	handleGamePadAxisYRot = input->registerJoyAxisMotionHandler(joystickNumber, XPAD_AXIS_YROT, bind(&ComponentUserControllable::onJoystickAxisYRot, this, _1));

	if(joystickNumber == 0)
	{
		handleKeyboardF = input->registerKeyPressHandler(SDLK_f, bind(&ComponentUserControllable::onKeyPressSuicide, this));
		handleKeyboardE = input->registerKeyPressHandler(SDLK_e, bind(&ComponentUserControllable::onKeyPressUse, this));
		handleKeyboardSpace_down = input->registerKeyPressHandler(SDLK_SPACE, bind(&ComponentUserControllable::onKeyPressChargeUp, this));
		handleKeyboardSpace_up = input->registerKeyUpHandler(SDLK_SPACE, bind(&ComponentUserControllable::onKeyPressAttack, this));

		handleKeyboardW = input->registerKeyDownHandler(SDLK_w, bind(&ComponentUserControllable::onKeyDownFwd, this));
		handleKeyboardS = input->registerKeyDownHandler(SDLK_s, bind(&ComponentUserControllable::onKeyDownRev, this));
		handleKeyboardA = input->registerKeyDownHandler(SDLK_a, bind(&ComponentUserControllable::onKeyDownLeft, this));
		handleKeyboardD = input->registerKeyDownHandler(SDLK_d, bind(&ComponentUserControllable::onKeyDownRight, this));

		handleMouseMove = input->registerMouseMoveHandler(bind(&ComponentUserControllable::onMouseMove, this, _1, _2));
		handleMouseRightClick = input->registerMouseDownRightHandler(bind(&ComponentUserControllable::onMouseDownRight, this, _1, _2));
		handleMouseLeftDown = input->registerMouseDownLeftHandler(bind(&ComponentUserControllable::onMouseDownLeft, this, _1, _2));
		handleMouseLeftUp = input->registerMouseUpLeftHandler(bind(&ComponentUserControllable::onMouseUpLeft, this, _1, _2));
	}
}

int ComponentUserControllable::getPlayerNumber() const
{
	return playerNumber;
}

void ComponentUserControllable::onKeyPressAttack()
{
	getParentBlackBoard().relayMessage(MessagePerformAction(EndChargeUp));
}

void ComponentUserControllable::onKeyPressChargeUp()
{
	getParentBlackBoard().relayMessage(MessagePerformAction(BeginChargeUp));
}

void ComponentUserControllable::releaseInputHandlers()
{
	ASSERT(input, "Null pointer: input");

	if(handleGamePadBtnR_down != -1)   input->unregisterJoyButtonPressHandler(handleGamePadBtnR_down);
	if(handleGamePadBtnR_up != -1)     input->unregisterJoyButtonUpHandler(handleGamePadBtnR_up);
	if(handleGamePadBtnL_down != -1)   input->unregisterJoyButtonPressHandler(handleGamePadBtnL_down);
	if(handleGamePadBtnL_up != -1)     input->unregisterJoyButtonUpHandler(handleGamePadBtnL_up);
	if(handleGamePadBtnY != -1)        input->unregisterJoyButtonPressHandler(handleGamePadBtnY);
	if(handleGamePadAxisX != -1)       input->unregisterJoyAxisMotionHandler(handleGamePadAxisX);
	if(handleGamePadAxisY != -1)       input->unregisterJoyAxisMotionHandler(handleGamePadAxisY);
	if(handleGamePadAxisXRot != -1)    input->unregisterJoyAxisMotionHandler(handleGamePadAxisXRot);
	if(handleGamePadAxisYRot != -1)    input->unregisterJoyAxisMotionHandler(handleGamePadAxisYRot);

	if(handleKeyboardSpace_down != -1) input->unregisterKeyPressHandler(handleKeyboardSpace_down);
	if(handleKeyboardSpace_up != -1)   input->unregisterKeyUpHandler(handleKeyboardSpace_up);
	if(handleKeyboardF != -1)          input->unregisterKeyPressHandler(handleKeyboardF);
	if(handleKeyboardE != -1)          input->unregisterKeyPressHandler(handleKeyboardE);
	if(handleKeyboardW != -1)          input->unregisterKeyDownHandler(handleKeyboardW);
	if(handleKeyboardS != -1)          input->unregisterKeyDownHandler(handleKeyboardS);
	if(handleKeyboardA != -1)          input->unregisterKeyDownHandler(handleKeyboardA);
	if(handleKeyboardD != -1)          input->unregisterKeyDownHandler(handleKeyboardD);

	if(handleMouseMove != -1)          input->unregisterMouseMoveHandler(handleMouseMove);
	if(handleMouseLeftDown != -1)      input->unregisterMouseDownLeftHandler(handleMouseLeftDown);
	if(handleMouseLeftUp != -1)        input->unregisterMouseUpLeftHandler(handleMouseLeftUp);
	if(handleMouseRightClick != -1)    input->unregisterMouseDownRightHandler(handleMouseRightClick);
}

void ComponentUserControllable::handleMessagePositionHasBeenSet(Message &m)
{
	lastReportedPosition = m.getField<vec3>("position");
}

void ComponentUserControllable::onJoystickAxisXRot(int value)
{
	static const float joystickSensitivity = 128000.0f;
	float dFacingAngle = value / joystickSensitivity;
	MessageRequestCharacterTurn m(-dFacingAngle);
	getParentBlackBoard().relayMessage(m);
}

void ComponentUserControllable::onJoystickAxisYRot(int value)
{
	// Do Nothing
}

void ComponentUserControllable::onMouseMove(ivec2 xy, ivec2 xyrel)
{
	float dFacingAngle = xyrel.x / mouseSensitivity;
	MessageRequestCharacterTurn m(-dFacingAngle);
	getParentBlackBoard().relayMessage(m);
}

void ComponentUserControllable::load( const PropertyBag &data )
{
	mouseSensitivity = data.getFloat("mouseSensitivity");
}

void ComponentUserControllable::onKeyPressSuicide()
{
	getParentBlackBoard().relayMessage(MessagePerformAction(Suicide));
}
