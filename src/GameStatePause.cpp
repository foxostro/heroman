#include "stdafx.h"
#include "Effect.h"
#include "Dimmer.h"
#include "FileText.h"
#include "Application.h"
#include "GameStateRun.h"
#include "GameStatePause.h"

GameStatePause::GameStatePause(Kernel &_kernel,
                               vector<Joystick> &joysticks,
                               GameStateMachine &gameStateMachine,
                               GameStateRun &_stateRun,
                               shared_ptr<Font> _font,
							   bool &_quitFlag)
: GameState(_kernel, _stateRun),
  text("PAUSE"),
  font(_font),
  input(joysticks),
  stateRun(_stateRun),
  quitFlag(_quitFlag)
{
	input.setApplicationQuitFlag(&_quitFlag);

	// [Q] key quits the game
	input.registerKeyPressHandler(SDLK_q, bind(&GameStatePause::quit, this));

	// [P] key pauses the game
	input.registerKeyPressHandler(SDLK_p, bind(&GameStateMachine::popState, &gameStateMachine));

	// Every joystick may unpause the game
	for(int i=0, numOfJoysticks = SDL_NumJoysticks(); i<numOfJoysticks; ++i)
	{
		input.registerJoyButtonPressHandler(i,  XPAD_BTN_START, bind(&GameStateMachine::popState, &gameStateMachine));
	}
}

void GameStatePause::update(float deltaTime)
{
	input.update();
	draw();
}

void GameStatePause::onEnter()
{
	GameState::onEnter();
}

void GameStatePause::draw() const
{
	ASSERT(font, "Null pointer: font");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GameState::draw();

	effect_Begin(effect_GUI);

	glPushMatrix();
	glTranslatef(400.0f, 400.0f, 0.1f);
	font->write(text, white, Font::SizeHuge);
	glPopMatrix();

	effect_End();
}

void GameStatePause::quit()
{
	quitFlag = true;
}
