#include "stdafx.h"

#include "SDLinput.h"
#include "GameStateMachine.h"
#include "MenuMain.h"

MenuMain::MenuMain(vector<Joystick> &_joysticks,
				   Kernel &_kernel,
				   GameStateMachine &_gameStateMachine,
				   GameStateRun &_stateRun,
				   BlackBoard &_blackBoard,
				   shared_ptr<Font> _font,
				   bool &_quitFlag)
: Menu(_joysticks, _kernel, _gameStateMachine,
	   _stateRun, _blackBoard, _font, _quitFlag),
  quitFlag(_quitFlag)
{
	populateElements();
}

void MenuMain::populateElements(void)
{
	elements.clear();
	elements.push_back( Element(vec2(200.0f, 500.0f), "Play",         true) );
	elements.push_back( Element(vec2(200.0f, 400.0f), "Credits",      true) );
	elements.push_back( Element(vec2(200.0f, 300.0f), "Instructions", true) );
	elements.push_back( Element(vec2(200.0f, 200.0f), "Quit",         true) );
}

void MenuMain::activateElement(int selectedIndex)
{
	const StateMachine::STATE &LEVEL_SELECT = gameStateMachine.LEVEL_SELECT;
	const StateMachine::STATE &CREDITS = gameStateMachine.CREDITS;
	const StateMachine::STATE &INSTRUCTIONS = gameStateMachine.INSTRUCTIONS;

	switch(selectedIndex)
	{
	case 0:
		gameStateMachine.pushState(LEVEL_SELECT);
		playMenuActivateSound();
		break;

	case 1:
		gameStateMachine.pushState(CREDITS);
		playMenuActivateSound();
		break;

	case 2:
		gameStateMachine.pushState(INSTRUCTIONS);
		playMenuActivateSound();
		break;

	case 3:
		quitFlag = true;
		break;
	};

	populateElements();
}
