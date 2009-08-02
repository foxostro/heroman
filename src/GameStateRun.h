#ifndef _GAME_STATE_RUN_H_
#define _GAME_STATE_RUN_H_

#include "SDLinput.h"
#include "Camera.h"
#include "gui.h"
#include "Dimmer.h"
#include "PerformanceLabel.h"
#include "GameStateMachine.h"
#include "World.h"

/** Normal running state of the game */
class GameStateRun : public GameStateMachine::GameState
{
public:
	/**
	Constructor
	@param frameTimer Calculates FPS and frame-rate
	@param kernel Game task kernel
	@param graphicsDevice Tracks major graphics device state
	@param textureFactory Texture factory tracks loaded textures
	@param gameStateMachine Game state machine that contains this game state
	@param gui Widget layer
	@param font Font in which to print text for objects in the world
	            (This is allowed to be different than the font set in gui)
	@param quitFlag The application allows quit requests to be written to this flag
	*/
	GameStateRun(FrameTimer &frameTimer,
		         Kernel &kernel,
				 vector<Joystick> &_joysticks,
				 GraphicsDevice &graphicsDevice,
				 TextureFactory &textureFactory,
				 BlackBoard &blackBoard,
				 GameStateMachine &gameStateMachine,
	             GUI &gui,
				 shared_ptr<Font> font,
	             bool &quitFlag);

	/**
	Updates the task every tick as long as the task has not been frozen
	@param deltaTime The seconds since the last tick
	*/
	virtual void update(float deltaTime);

	/** Gets the game world */
	inline const World& getWorld() const
	{
		ASSERT(world, "Member \"world\" was null");
		return *world;
	}

	/** Gets the game world */
	inline World& getWorld()
	{
		ASSERT(world, "Member \"world\" was null");
		return *world;
	}

	/** Draw the game */
	virtual void draw() const;

	/**
	Take whatever the screen dimness is and transition from that to
	the value intended for this particular state.
	*/
	virtual void onEnter();

private:
	/**
	Display debug data and FPS, as appropriate
	@param frameLength Seconds since the last frame
	@param font Font in which to print the debug data
	*/
	void displayDebugText(float frameLength, Font &font) const;

	/** Quits the game */
	void quit();

private:
	SDLinput input;

	GUI &gui;

	Dimmer dim;

	PerformanceLabel *performanceLabel;

	shared_ptr<World> world;

	/** The application allows quit requests to be written to this flag */
	bool &quitFlag;
};

#endif
