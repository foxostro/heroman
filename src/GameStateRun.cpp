#include "stdafx.h"
#include "Application.h"
#include "Effect.h"
#include "Camera.h"
#include "ScreenShot.h"
#include "GameStateRun.h"

GameStateRun::GameStateRun(FrameTimer &frameTimer,
						   Kernel &_kernel,
						   vector<Joystick> &joysticks,
						   GraphicsDevice &graphicsDevice,
						   TextureFactory &textureFactory,
						   BlackBoard &blackBoard,
						   GameStateMachine &gameStateMachine,
						   GUI &_gui,
						   shared_ptr<Font> font,
						   bool &_quitFlag)
: GameState(_kernel, 666.7f, 0.0f),
  input(joysticks),
  gui(_gui),
  performanceLabel(0),
  quitFlag(_quitFlag)
{
	// [ESC] key goes back to the menu
	input.setApplicationQuitFlag(&_quitFlag);
	input.registerKeyPressHandler(SDLK_ESCAPE, bind(&GameStateMachine::popState, &gameStateMachine));

	// [P] key pauses the game
	input.registerKeyPressHandler(SDLK_p, bind(&GameStateMachine::pushState, &gameStateMachine, gameStateMachine.PAUSE));

	// [Q] key quits the game
	input.registerKeyPressHandler(SDLK_q, bind(&GameStateRun::quit, this));

	// Every joystick may pause the game
	for(int i=0, numOfJoysticks = SDL_NumJoysticks(); i<numOfJoysticks; ++i)
	{
		input.registerJoyButtonPressHandler(i,  XPAD_BTN_START, bind(&GameStateMachine::pushState, &gameStateMachine, gameStateMachine.PAUSE));
	}

	// [F11] takes a screen shot
	input.registerKeyPressHandler(SDLK_F11, bind(&takeScreenShot, string("screen")));

	// Create a performance label to display FPS
	performanceLabel = new PerformanceLabel(vec2(60,60), font, frameTimer);
	performanceLabel->visible = false;
	gui.addWidget(performanceLabel);

	// Create the game world
	world = shared_ptr<World>(new World(BlackBoard::genName(),
	                                    &blackBoard,
	                                    graphicsDevice, 
	                                    textureFactory,
	                                    input,
	                                    font,
										gui));
	blackBoard.registerSubscriber(world.get());

	// Loads the game world
	getWorld().loadFromFile(FileName("data/maps/level1.xml"));
}

void GameStateRun::update(float deltaTime)
{
	input.update();
	getWorld().update(deltaTime);
	gui.update(deltaTime);

	draw();
}

void GameStateRun::draw() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	getWorld().draw();
	dim.draw();
	gui.draw();
}

void GameStateRun::displayDebugText(float frameLength, Font &font) const
{
	effect_Begin(effect_GUI);

	{
		const string frameLengthText = fitToFieldSize(ftos(frameLength * 1000.0f), ' ', 4, JUSTIFY_LEFT);
		const string fpsText = ftos(1.0f / frameLength);

		glPushMatrix();
		glTranslatef(50.0f, 700.0f, 0.1f);
		font.write(frameLengthText + " ms  ;  " + fpsText + " fps", white, Font::SizeLarge);
		glPopMatrix();
	}

	effect_End();
}

void GameStateRun::onEnter()
{
	//getWorld().playersEnter(1);
	GameStateMachine::GameState::onEnter();
}

void GameStateRun::quit()
{
	quitFlag = true;
}
