#include "stdafx.h"
#include "SDLinput.h"
#include "GameStateMachine.h"
#include "GameStateRun.h"
#include "MenuLevelSelect.h"

MenuLevelSelect::MenuLevelSelect(vector<Joystick> &_joysticks,
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
	levels[1] = make_tuple("Level 1", FileName("data/maps/level1.xml"));
	levels[2] = make_tuple("Level 2", FileName("data/maps/level2.xml"));
	levels[3] = make_tuple("Level 3", FileName("data/maps/level3.xml"));

	populateElements();
}

void MenuLevelSelect::populateElements(void)
{
	float x = 200;

	int numOfJoysticks = SDL_NumJoysticks();
	elements.clear();
	elements.push_back(Element(vec2(x,500.0f),"Back",true));

	float y = 400;
	for(Levels::const_iterator i=levels.begin(); i!=levels.end(); ++i, y-=100)
	{
		string s = i->second.get<0>();
		elements.push_back(Element(vec2(x,y),s,true));
	}
}

void MenuLevelSelect::activateElement(int selectedIndex)
{
	switch(selectedIndex)
	{
	case 0:
		playMenuActivateSound();
		gameStateMachine.popState();
		break;

	default:
		{
			FileName fileName = levels[selectedIndex].get<1>();
			playMenuActivateSound();
			getStateRun()->getWorld().loadFromFile(fileName);

			if(SDL_NumJoysticks() == 0)
			{
				getStateRun()->getWorld().playersEnter(1);
				gameStateMachine.pushState(gameStateMachine.RUN);
			}
			else
			{
				gameStateMachine.pushState(gameStateMachine.PLAYER_SELECT);
			}
		} break;
	};

	populateElements();
}
