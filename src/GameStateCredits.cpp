#include "stdafx.h"
#include "Effect.h"
#include "Dimmer.h"
#include "FileText.h"
#include "Application.h"
#include "GameStateRun.h"
#include "GameStateCredits.h"

GameStateTextScreen::GameStateTextScreen(Kernel &_kernel,
								   vector<Joystick> &joysticks,
								   GameStateMachine &_gameStateMachine,
								   GameStateRun &_stateRun,
								   shared_ptr<Font> _font,
								   bool &_quitFlag,
								   const FileName &textFile,
								   bool scrollText)
: GameState(_kernel, _stateRun),
  text("failed to load"),
  scrollSpeed(0.07f),
  y(600.0f),
  font(_font),
  input(joysticks),
  stateRun(_stateRun)
{
	if(!scrollText)
	{
		scrollSpeed=0.0f;
	}

	// Load credits text
	FileText file;
	if(file.openStream(textFile, File::FILE_MODE_READ))
	{
		text = file.getFullText();
	}

	// [ESC] key exits the credits screen
	input.setApplicationQuitFlag(&_quitFlag);
	input.registerKeyPressHandler(SDLK_ESCAPE, bind(&GameStateMachine::popState, &_gameStateMachine));
	input.registerJoyButtonPressHandler(0, XPAD_BTN_START, bind(&GameStateMachine::popState, &_gameStateMachine));
	input.registerJoyButtonPressHandler(0, XPAD_BTN_BACK, bind(&GameStateMachine::popState, &_gameStateMachine));
}

void GameStateTextScreen::update(float deltaTime)
{
	input.update();
	y += scrollSpeed * deltaTime; // scroll a bit
	draw();
}

void GameStateTextScreen::onEnter()
{
	y = 500.0f;
	GameState::onEnter();
}

void GameStateTextScreen::draw() const
{
	ASSERT(font, "Null pointer: font");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GameState::draw();

	effect_Begin(effect_GUI);

	glPushMatrix();
	glTranslatef(200.0f, y, 0.1f);
	font->write(text, white, Font::SizeHuge);
	glPopMatrix();

	effect_End();
}
