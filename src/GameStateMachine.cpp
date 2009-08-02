#include "stdafx.h"
#include "Kernel.h"
#include "LinearInterpolator.h"
#include "GameStateRun.h"
#include "GameStateCredits.h"
#include "GameStatePause.h"
#include "MenuMain.h"
#include "MenuPlayerSelect.h"
#include "MenuLevelSelect.h"
#include "GameStateMachine.h"

GameStateMachine::GameState::GameState(Kernel &_kernel,
		                               GameStateRun &_stateRun,
		                               float _transitionTime,
		                               float _dimness)
: kernel(_kernel),
  stateRun(&_stateRun),
  transitionTime(_transitionTime),
  dimness(_dimness)
{}

GameStateMachine::GameState::GameState(Kernel &_kernel,
		                               float _transitionTime,
		                               float _dimness)
: kernel(_kernel),
  stateRun(0),
  transitionTime(_transitionTime),
  dimness(_dimness)
{}

void GameStateMachine::GameState::draw() const
{
	if(stateRun)
	{
		const World &world = stateRun->getWorld();
		world.draw();
	}

	dimmer.draw();
}

void GameStateMachine::GameState::onEnter()
{
	if(transitionTime>0.0f)
	{
		const float start = Dimmer::alphaBlur;
		const float end = dimness;

		kernel.addTask(new LinearInterpolator(&Dimmer::alphaBlur,
			                                  start,
			                                  end,
			                                  transitionTime));
	}
	else
	{
		Dimmer::alphaBlur = dimness;
	}
}

GameStateMachine::GameStateMachine(BlackBoardUID _uid,
								   BlackBoard &parentBlackBoard,
								   vector<Joystick> &joysticks,
								   FrameTimer &_frameTimer,
								   shared_ptr<Font> _font,
								   GraphicsDevice &_graphicsDevice,
								   TextureFactory &_textureFactory,
								   Kernel &_kernel,
								   GUI &_gui,
								   bool &_quitFlag)
: StateMachine(_uid, &parentBlackBoard),
  RUN(NULL_STATE),
  CREDITS(NULL_STATE),
  INSTRUCTIONS(NULL_STATE),
  COMPLETE(NULL_STATE),
  MENU(NULL_STATE),
  GAMEOVER(NULL_STATE),
  PAUSE(NULL_STATE),
  PLAYER_SELECT(NULL_STATE),
  LEVEL_SELECT(NULL_STATE)
{
	registerMessageHandler("MessageGoalHasBeenCompleted", bind(&GameStateMachine::handleMessageGoalHasBeenCompleted, this, _1));
	registerMessageHandler("MessageDeclareGameOver", bind(&GameStateMachine::handleMessageDeclareGameOver, this, _1));

	// generate state names
	RUN           = genNewStateName();
	CREDITS       = genNewStateName();
	INSTRUCTIONS  = genNewStateName();
	COMPLETE      = genNewStateName();
	MENU          = genNewStateName();
	GAMEOVER      = genNewStateName();
	PAUSE         = genNewStateName();
	PLAYER_SELECT = genNewStateName();
	LEVEL_SELECT  = genNewStateName();
	TRACE("Generated state names (locally)");

	{
		// Create states
		GameStateRun *stateRun = new GameStateRun(_frameTimer, _kernel, joysticks, _graphicsDevice, _textureFactory, parentBlackBoard, *this, _gui, _font, _quitFlag);
		GameStateTextScreen *stateCredits = new GameStateTextScreen(_kernel, joysticks, *this, *stateRun, _font, _quitFlag, FileName("data/text/credits.txt"), true);
		GameStateTextScreen *stateInstructions = new GameStateTextScreen(_kernel, joysticks, *this, *stateRun, _font, _quitFlag, FileName("data/text/instructions.txt"), false);
		GameStateTextScreen *stateComplete = new GameStateTextScreen(_kernel, joysticks, *this, *stateRun, _font, _quitFlag, FileName("data/text/complete.txt"), false);
		MenuMain *stateMenu = new MenuMain(joysticks, _kernel, *this, *stateRun, parentBlackBoard, _font, _quitFlag);
		GameStateTextScreen *stateGameOver = new GameStateTextScreen(_kernel, joysticks, *this, *stateRun, _font, _quitFlag, FileName("data/text/gameover.txt"), false);
		GameStatePause *statePause = new GameStatePause(_kernel, joysticks, *this, *stateRun, _font, _quitFlag);
		MenuPlayerSelect *statePlayerSelect = new MenuPlayerSelect(joysticks, _kernel, *this, *stateRun, parentBlackBoard, _font, _quitFlag);
		MenuLevelSelect *stateLevelSelect = new MenuLevelSelect(joysticks, _kernel, *this, *stateRun, parentBlackBoard, _font, _quitFlag);
		TRACE("Created game state objects");

		// bind game state names
		bindState(RUN, stateRun);
		bindState(CREDITS, stateCredits);
		bindState(INSTRUCTIONS, stateInstructions);
		bindState(COMPLETE, stateComplete);
		bindState(MENU, stateMenu);
		bindState(GAMEOVER, stateGameOver);
		bindState(PAUSE, statePause);
		bindState(PLAYER_SELECT, statePlayerSelect);
		bindState(LEVEL_SELECT, stateLevelSelect);
		TRACE("Bound game state names");
	}

	pushState(MENU);
	TRACE("Entered intial state");
}

void GameStateMachine::handleMessageGoalHasBeenCompleted(Message &message)
{
	while(popState());
	pushState(MENU);
	pushState(COMPLETE);
}

void GameStateMachine::handleMessageDeclareGameOver( Message &message )
{
	while(popState());
	pushState(MENU);
	pushState(GAMEOVER);
}
