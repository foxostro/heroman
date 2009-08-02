#ifndef _GAME_STATE_CREDITS_H_
#define _GAME_STATE_CREDITS_H_

#include "GameStateMachine.h"
#include "SDLinput.h"

/**
State of the game where some text is displayed.
Text may be specified as scrolling or not.
*/
class GameStateTextScreen : public GameStateMachine::GameState
{
public:
	/**
	Constructor
	@param kernel Game task kernel
	@param joysticks Joystick devices
	@param gameStateMachine Game state machine that contains this game state
	@param stateRun RUN state
	@param font Font in which credits should be printed
	@param quitFlag The application quits when this flag is raised
	@param textFile Text file to load for this screen
	@param scrollText when true, scroll the text
	*/
	GameStateTextScreen(Kernel &kernel,
		                vector<Joystick> &joysticks,
					    GameStateMachine &gameStateMachine,
		                GameStateRun &stateRun,
					    shared_ptr<Font> font,
					    bool &quitFlag,
						const FileName &textFile,
						bool scrollText);

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

private:
	/** The text to scroll through */
	string text;

	/** Rate of scrolling through the text */
	float scrollSpeed;

	/** Current position of the scrolling text */
	float y;

	/** Font in which credits should be printed */
	shared_ptr<Font> font;

	/** Manages interface to SDL for input */
	SDLinput input;

	/** Access to the game world */
	GameStateRun &stateRun;
};

#endif
