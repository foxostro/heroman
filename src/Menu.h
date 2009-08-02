#ifndef _MENU_H_
#define _MENU_H_

#include "vec2.h"
#include "SDLinput.h"
#include "GameStateMachine.h"
#include "BlackBoard.h"

class Font;
class GameStateRun;

/** Game menu screen */
class Menu : public GameStateMachine::GameState
{
public:
	/** Menu item */
	class Element
	{
	private:
		/** Position of the menu item on the menu screen */
		vec2 position;

		/** Label for the menu item */
		string label;

		/** Indicates that the item is enabled */
		bool enabled;

	public:
		/**
		Constructor
		@param position Position of the menu item on the menu screen
		@param label Label for the menu item
		@param enabled Indicates that the element is enabled
		*/
		Element(const vec2 &position, const string &label, bool enabled=true)
		{
			this->position = position;
			this->label = label;
			this->enabled = enabled;
		}

		/**
		Gets the position of the menu item
		@return The position of the menu item
		*/
		const vec2& getPosition() const
		{
			return position;
		}

		/**
		Gets the label for the menu item
		@return The label for the menu item
		*/
		const string& getLabel() const
		{
			return label;
		}

		/**
		Determines whether the menu item is enabled
		@return true if the menu item is enabled
		*/
		bool isEnabled() const
		{
			return enabled;
		}

		/** Enables the menu item */
		void enable()
		{
			enabled = true;
		}

		/** Disables the menu item */
		void disable()
		{
			enabled = false;
		}
	};

public:
	/**
	Constructor
	@param joysticks Joystick devices
	@param kernel Game task kernel
	@param gameStateMachine Game state machine that contains this game state
	@param stateRun Access to the game world
	@param blackBoard Communication between game subsystems
	@param font Font in which to print the menu
	@param quitFlag The application allows quit requests to be written to this flag
	*/
	Menu(vector<Joystick> &joysticks,
		 Kernel &kernel,
		 GameStateMachine &gameStateMachine,
		 GameStateRun &stateRun,
		 BlackBoard &blackBoard,
		 shared_ptr<Font> font,
		 bool &quitFlag);

	/** Destructor */
	virtual ~Menu();

	/**
	Updates the task every tick as long as the task has not been frozen
	@param deltaTime The seconds since the last tick
	*/
	virtual void update(float deltaTime);

	/** Draw the menus */
	virtual void draw() const;

protected:
	/** re-populates the menu elements */
	virtual void populateElements() = 0;

	/**
	Activates an element of the menu
	@param The selected element to activate
	*/
	virtual void activateElement(int selectedIndex) = 0;

	void playSound(const FileName &sound);
	void playMenuUpSound();
	void playMenuDownSound();
	void playMenuActivateSound();
	void playMenuDisabledSound();

private:
	void keypressMenuUp();
	void keypressMenuDown();
	void keypressMenuEnter();

private:
	/** The index of the currently selected element */
	int selectedIndex;

	/** The current font */
	shared_ptr<Font> font;

protected:
	/** Blackboard for subsystem communication */
	BlackBoard &blackBoard;

	/** The menu elements */
	vector<Element> elements;

	/** Game state machine that contains this game state */
	GameStateMachine &gameStateMachine;

	/** Manages interface to SDL for input */
	SDLinput input;
};

#endif
