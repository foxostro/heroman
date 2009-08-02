#include "stdafx.h"
#include "FileFuncs.h"
#include "Font.h"
#include "World.h"
#include "Actor.h"
#include "ScreenShot.h"
#include "Application.h"
#include "AnimationControllerFactory.h"

shared_ptr<AnimationControllerFactory> g_ModelFactory;

/**
Application entry-point (e.g. WinMain or main) immediately calls
this function to handle everything.
*/
int mymain()
{
	Application *app = new Application();
	app->start();
	app->run();
	app->destroy(); //delete app;
	return EXIT_SUCCESS;
}

Application::Application()
{
	clear();
}

Application::~Application()
{
	destroy();
}

void Application::initializeFonts()
{
	const PropertyBag data = PropertyBag::fromFile(FileName("data/fonts/fonts.xml")).getBag("fonts");

	for(size_t i=0, n=data.getNumInstances("font"); i<n; ++i)
	{
		const PropertyBag fontData = data.getBag("font", i);
		fonts.insert(make_pair(fontData.getString("name"),
			                   shared_ptr<Font>(new Font(fontData))));
	}

    font = fonts[data.getString("default")];
}

void Application::initializeSharedActorFactory()
{
	AnimationControllerFactory *factory = new AnimationControllerFactory(textureFactory);
	g_ModelFactory = shared_ptr<AnimationControllerFactory>(factory);
}

void Application::initializeFrameTimer()
{
	frameTimer = shared_ptr<FrameTimer>(new FrameTimer);
	TRACE("Created the frame timer");
}

void Application::initializeSoundManager()
{
	soundSystem = shared_ptr<SoundSystem>(new SoundSystem(blackBoard.genName(), &blackBoard));
	soundSystem->setMute(false);
	soundSystem->setSoundEffectVolume(0.7f);
	soundSystem->setMusicVolume(0.0f);
	soundSystem->playMusic(FileName("data/music/RachelBerkowitz.mp3"));
	blackBoard.registerSubscriber(soundSystem.get());
	TRACE("Sound system initialized");
}

void Application::initializeGameStateMachine()
{
	GameStateMachine *s = new GameStateMachine(blackBoard.genName(),
											   blackBoard,
	                                           joysticks,
	                                           *frameTimer,
		                                       font,
											   *graphicsDevice,
											   textureFactory,
											   kernel,
											   gui,
											   quitRequested);

	gameStateMachine = shared_ptr<GameStateMachine>(s);
	blackBoard.registerSubscriber(gameStateMachine.get());

	TRACE("Game state machine initialized");
}

void Application::start()
{
    TRACE("Starting application...");

    setWorkingDirectory(getApplicationDirectory());
	SDL_Init(SDL_INIT_EVERYTHING);
	dInitODE();
	initializeGraphicsDevice();
	initializeDevIL();
	
	initializeJoystickDevices();
	initializeSharedActorFactory();
	initializeFonts();
	srand(SDL_GetTicks());
	initializeFrameTimer();
	initializeSoundManager();
	initializeGameStateMachine();

	TRACE("Application start-up completed");
}

void Application::initializeDevIL()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
}

void Application::closeJoysticks()
{
	for(vector<Joystick>::iterator i=joysticks.begin();
		i!=joysticks.end();
		++i)
	{
		SDL_Joystick *joystick = i->handle;

		if(joystick)
		{
			SDL_JoystickClose(joystick);
		}
	}

	joysticks.clear();
}

void Application::initializeJoystickDevices()
{
	closeJoysticks();

	const int numJoysticks = SDL_NumJoysticks();
	for(int which=0; which<numJoysticks; ++which)
	{
		SDL_Joystick *handle = SDL_JoystickOpen(which);

		if(handle)
		{
			TRACE(string("Joystick ") + SDL_JoystickName(which) + " Successfully opened.");
		}

		joysticks.push_back(Joystick(handle, which));
	}
}

float Application::updateFrameTimer()
{
	ASSERT(frameTimer, "Frame timer was null");

	if(movieMode)
	{
		const float thirty = 1.0f / 30.0f * 1000.0f;

		// Lock frame rate
		while((float)frameTimer->getElapsedTime() < thirty);

		frameTimer->update();

		return thirty;
	}
	else
	{
		// Lock frame rate at 30 fps
		const float thirty = 1.0f / 30.0f * 1000.0f;
		while((float)frameTimer->getElapsedTime() < thirty); 
		frameTimer->update();
		return thirty;
	}
}

void Application::run()
{
	for(float frameLength=0.0f; !quitRequested; frameLength=updateFrameTimer())
	{
		ASSERT(gameStateMachine, "Member \"gameStateMachine\" was null");
		ASSERT(graphicsDevice,   "Member \"graphicsDevice\" was null");

		gameStateMachine->update(frameLength);
		kernel.update(frameLength);
		graphicsDevice->finishFrame();

		if(movieMode)
		{
			takeScreenShot("arfox");
		}
	}
}

void Application::destroy()
{
	TRACE("Shutting down application...");

	kernel.destroy();
	TRACE("Kernel has been shutdown");

	soundSystem.reset();
	TRACE("Sound subsystem has been shutdown");

	graphicsDevice.reset();
	TRACE("Graphics device has been shutdown");

	dCloseODE();
	TRACE("Physics subsystem has been shutdown");

	SDL_Quit();
	TRACE("SDL libraries have been shutdown");

	clear();
	TRACE("...shutdown completed");
}

void Application::clear()
{
	quitRequested = false;
	movieMode = false;
	soundSystem.reset();
	frameTimer.reset();
}

void Application::initializeGraphicsDevice()
{
	bool fullscreen	= false;
	ivec2 windowSize(640, 480);
	GraphicsDevice *gd = new GraphicsDevice(windowSize, fullscreen);
	graphicsDevice = shared_ptr<GraphicsDevice>(gd);

	TRACE("Graphics device initialized");
}
