#ifndef _GAME_STATE_PAUSE_H_
#define _GAME_STATE_PAUSE_H_

#include "GameStateMachine.h"
#include "SDLinput.h"

/** Paused game state */
class GameStatePause : public GameStateMachine::GameState
{
public:
	/**
	Constructor
	@param kernel Game task kernel
	@param joysticks Joystick devices
	@param gameStateMachine Game state machine that contains this game state
	@param stateRun RUN state
	@param font Font in which credits should be printed
	*/
	GameStatePause(Kernel &kernel,
	               vector<Joystick> &joysticks,
	               GameStateMachine &gameStateMachine,
	               GameStateRun &stateRun,
	               shared_ptr<Font> font,
				   bool &quitFlag);

	/**
	Updates the task every tick as long as the task has not been frozen
	@param deltaTime The seconds since the last tick
	*/
	virtual void update(float deltaTime);

	/** Called when the state is entered */
	virtual void onEnter();

protected:
	/** Draws the screen */
	void draw() const;

	/** Quits the game */
	void quit();

private:
	/** The text to scroll through */
	string text;

	/** Font in which credits should be printed */
	shared_ptr<Font> font;

	/** Manages interface to SDL for input */
	SDLinput input;

	/** Access to the game world */
	GameStateRun &stateRun;

	/** The application allows quit requests to be written to this flag */
	bool &quitFlag;
};

#endif
