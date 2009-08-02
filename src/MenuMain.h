#ifndef _MENU_MAIN_H_
#define _MENU_MAIN_H_

#include "Menu.h"

class GameStateMachine;
class GameStateRun;
class Font;

/** Main game menu screen */
class MenuMain : public Menu
{
public:
	/**
	Constructor
	@param kernel Access to the game  task kernel
	@param gameStateMachine Game state machine that contains this game state
	@param stateRun RUN state
	@param soundSystem Sound system so that we may play menu sound effects
	@param font Font in which to print the menu
	@param quitFlag The application allows quit requests to be written to this flag
	*/
	MenuMain(vector<Joystick> &joysticks,
	         Kernel &kernel,
		     GameStateMachine &gameStateMachine,
		     GameStateRun &stateRun,
		     BlackBoard &blackBoard,
			 shared_ptr<Font> font,
			 bool &quitFlag);

protected:
	/** re-populates the menu elements */
	virtual void populateElements(void);

	/**
	Activates an element of the menu
	@param selectedIndex The selected element to activate
	*/
	virtual void activateElement(int selectedIndex);

private:
	/** The application allows quit requests to be written to this flag */
	bool &quitFlag;
};

#endif
