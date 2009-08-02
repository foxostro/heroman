#include "stdafx.h"

#include "SDLinput.h"
#include "GameStateMachine.h"
#include "GameStateRun.h"
#include "MenuPlayerSelect.h"

MenuPlayerSelect::MenuPlayerSelect(vector<Joystick> &_joysticks,
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

void MenuPlayerSelect::populateElements(void)
{
	int numOfJoysticks = SDL_NumJoysticks();
	elements.clear();
	elements.push_back(Element(vec2(200.0f,500.0f),"Back",true));

	if(numOfJoysticks==0)
	{
		elements.push_back(Element(vec2(200.0f,400.0f),"Single Player",true));
	}
	else
	{
		elements.push_back(Element(vec2(200.0f,400.0f),"Single Player",true));

		for(int i=1; i<4; ++i)
		{
			float y = 400.0f - i*100.0f;
			string s = itos(i+1)+" Players";
			elements.push_back(Element(vec2(200.0f,y),s,i<numOfJoysticks));
		}
	}
}

void MenuPlayerSelect::activateElement(int selectedIndex)
{
	const StateMachine::STATE &RUN = gameStateMachine.RUN;
	const StateMachine::STATE &CREDITS = gameStateMachine.CREDITS;
	const StateMachine::STATE &INSTRUCTIONS = gameStateMachine.INSTRUCTIONS;

	switch(selectedIndex)
	{
	case 0:
		gameStateMachine.popState();
		playMenuActivateSound();
		break;

	default:
		{
			getStateRun()->getWorld().playersEnter(selectedIndex);
			gameStateMachine.pushState(RUN);
		}
		break;
	};

	populateElements();
}
