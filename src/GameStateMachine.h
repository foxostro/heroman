#ifndef _GAME_STATE_MACHINE_H_
#define _GAME_STATE_MACHINE_H_

#include "StateMachine.h"
#include "Dimmer.h"
#include "BlackBoard.h"

class FrameTimer;
class GameStateRun;
class Kernel;
class TextureFactory;
class Font;
class GraphicsDevice;
class GUI;

/** State machine for game state and transitions */
class GameStateMachine : public StateMachine
{
public:
	/** Game State in a game state machine */
	class GameState : public State
	{
	public:
        virtual ~GameState(){}

		/**
		@param kernel Game task kernel
		@param stateRun Access to the game world
		@param transitionTime Time for the dimmer to transition
		@param dimness Intensity of the screen while in this game state
		*/
		GameState(Kernel &kernel,
			      GameStateRun &stateRun,
				  float transitionTime = 666.7f,
				  float dimness = 0.8);

		/**
		@param kernel Game task kernel
		@param transitionTime Time for the dimmer to transition
		value to disable the dimmer phase.
		@param dimness Intensity of the screen while in this game state
		*/
		GameState(Kernel &kernel,
			      float transitionTime = 666.7f,
			      float dimness = 0.8f);

		/** the screen dimmed world */
		virtual void draw() const;

		/**
		Take whatever the screen dimness is and transition from that to
		the value intended for this particular state.
		*/
		virtual void onEnter();

	protected:
		/** Gets the run state of the game (stored internally) */
		GameStateRun* getStateRun()
		{
			ASSERT(stateRun, "Null pointer: stateRun");
			return stateRun;
		}

	private:
		/** Game task kernel */
		Kernel &kernel;

		/** Access to the game world */
		GameStateRun *stateRun;

		/**
		Time for the dimmer to transition when a state transition occurs.
		Specify a zero or negative value to disable the dimmer phase.
		*/
		float transitionTime;

		/** Intensity of the screen while in this game state */
		float dimness;

		/** Screen Dimmer */
		Dimmer dimmer;
	};

	virtual ~GameStateMachine(){}

	/** Creates game states */
	GameStateMachine(BlackBoardUID uid,
	                 BlackBoard &parentBlackBoard,
			         vector<Joystick> &joysticks,
	                 FrameTimer &frameTimer,
	                 shared_ptr<Font> font,
					 GraphicsDevice &graphicsDevice,
					 TextureFactory &textureFactory,
					 Kernel &kernel,
					 GUI &gui,
					 bool &quitFlag);

	/** Game state names */
	STATE RUN, CREDITS, INSTRUCTIONS, COMPLETE,
	      MENU, GAMEOVER, PAUSE, PLAYER_SELECT,
		  LEVEL_SELECT;

private:
	void handleMessageGoalHasBeenCompleted(Message &message);
	void handleMessageDeclareGameOver(Message &message);
};

#endif
