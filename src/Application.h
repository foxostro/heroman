#ifndef APP_H
#define APP_H

#include "myassert.h"
#include "FrameTimer.h"
#include "Task.h"
#include "gui.h"
#include "SoundSystem.h"
#include "TextureFactory.h"
#include "Camera.h"
#include "GraphicsDevice.h"
#include "Kernel.h"
#include "SDLinput.h"
#include "BlackBoard.h"
#include "GameStateMachine.h"

/**
The Application class may be considered the entry point of the application.
While the main or WinMain functions (depending on the OS we are compiling for)
are used to enter the application from the Operating System kernel, the only
purpose they serve is to immediately instantiate an Application type object.
*/
class Application
{
public:
	Application();
	~Application();

	/** Run the game loop */
	void run();

	/** Initialization code to run before the game loop */
	void start();
	
	/** Shutdown code to run after the game loop exits */
	void destroy();

private:
	/** Reset the state of the application to a "just-constructed" state */
	void clear();

	/** Start DevIL */
	void initializeDevIL();

	/** Start OpenGL */
	void initializeGraphicsDevice();

	/** Closes all joysticks */
	void closeJoysticks();

	/** Initializes joystick devices */
	void initializeJoystickDevices();

	/** Load the font data files */
	void initializeFonts();

	/** Updates the frane timer and get the frame length */
	float updateFrameTimer();

	/** Initialize the shared actor factory */
	void initializeSharedActorFactory();

	/** Initialize the frame timer */
	void initializeFrameTimer();

	/** Initialize the sound module */
	void initializeSoundManager();

	/** Initialize the game state machine and states */
	void initializeGameStateMachine();

private:
	BlackBoard blackBoard;
	shared_ptr<Font> font;
	shared_ptr<FrameTimer> frameTimer;
	shared_ptr<GameStateMachine> gameStateMachine;
	shared_ptr<GraphicsDevice> graphicsDevice;
	shared_ptr<SoundSystem> soundSystem;
	bool quitRequested;
	Kernel kernel;
	GUI gui;
	TextureFactory textureFactory;
	vector<Joystick> joysticks;
	bool movieMode;
	map<string, shared_ptr<Font> > fonts;
};

#endif
