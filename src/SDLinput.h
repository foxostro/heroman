#ifndef _SDL_INPUT_H_
#define _SDL_INPUT_H_

#include "UniqueIdFactory.h"
#include "myassert.h"
#include "xpad.h"

class Application;

#define MAX_JOYSTICK_BUTTONS (64)
#define MAX_JOYSTICK_AXII (64)
#define MAX_JOYSTICKS (64)

/** State of a single joystick */
class Joystick
{
public:
	SDL_Joystick *handle;
	int which; /** Joystick index */
	bool btnState[MAX_JOYSTICK_BUTTONS];

	Joystick(SDL_Joystick *_handle, int _which)
		: handle(_handle),
		which(_which)
	{
		for(size_t i=0; i<sizeof(btnState); ++i)
		{
			btnState[i] = false;
		}
	}
};

/**
Handles SDL input events and passes them onto registered input handlers.

Only one SDLinput object should be in action at a time, as it empties out the
input event queue. However, a separate SDLinput object should be used for each
separate game state as event handlers are ususally only applicable to some
specific context.

For example, the player movement callbacks should not be handled while in the
game menu, and the menu navigation callbacks should not be called when outside
the game menu.

@author Andrew Fox.  This class shares the same name and purpose as one
        written by Tom Cauchois for Parasomnia and used again for Arbarlith2.
		However, this class uses none of Tom's code.
*/
class SDLinput
{
public:
	typedef function <void () > EventKeyDown;
	typedef function <void () > EventKeyUp;
	typedef function <void () > EventKeyPress;
	typedef function <void (ivec2, ivec2) > EventMouseMove;
	typedef function <void (int, int) > EventMouseDownLeft;
	typedef function <void (int, int) > EventMouseDownRight;
	typedef function <void (int, int) > EventMouseUpLeft;
	typedef function <void (int, int) > EventMouseUpRight;
	typedef function <void () > EventJoyButtonDown;
	typedef function <void () > EventJoyButtonUp;
	typedef function <void () > EventJoyButtonPress;
	typedef function <void (int) > EventJoyAxisMotion;
	typedef int HANDLE;

	/** Destructor */
	~SDLinput();

	/** Constructor */
	SDLinput(vector<Joystick> &joysticks);

	/** Polls for input */
	void update();

	/**
	Allows the input subsystem to convey SDL_QUIT events to the application
	@param quitFlag The application allows quit requests to be written to this flag
	*/
	void setApplicationQuitFlag(bool *quitFlag)
	{
		this->quitFlag = quitFlag;
	}

	HANDLE registerKeyDownHandler(SDLKey key, const EventKeyDown &handler);
	HANDLE registerKeyUpHandler(SDLKey key, const EventKeyUp &handler);
	HANDLE registerKeyPressHandler(SDLKey key, const EventKeyPress &handler);

	HANDLE registerMouseMoveHandler(const EventMouseMove &handler);
	HANDLE registerMouseDownLeftHandler(const EventMouseDownLeft &handler);
	HANDLE registerMouseDownRightHandler(const EventMouseDownRight &handler);
	HANDLE registerMouseUpLeftHandler(const EventMouseUpLeft &handler);
	HANDLE registerMouseUpRightHandler(const EventMouseUpRight &handler);

	HANDLE registerJoyAxisMotionHandler(int joystick, int axis, const EventJoyAxisMotion &handler);
	HANDLE registerJoyButtonDownHandler(int joystick, int button, const EventJoyButtonDown &handler);
	HANDLE registerJoyButtonUpHandler(int joystick, int button, const EventJoyButtonUp &handler);
	HANDLE registerJoyButtonPressHandler(int joystick, int button, const EventJoyButtonPress &handler);

	void unregisterKeyDownHandler(HANDLE handle);
	void unregisterKeyUpHandler(HANDLE handle);
	void unregisterKeyPressHandler(HANDLE handle);

	void unregisterMouseMoveHandler(HANDLE handle);
	void unregisterMouseDownLeftHandler(HANDLE handle);
	void unregisterMouseDownRightHandler(HANDLE handle);
	void unregisterMouseUpLeftHandler(HANDLE handle);
	void unregisterMouseUpRightHandler(HANDLE handle);

	void unregisterJoyAxisMotionHandler(HANDLE handle);
	void unregisterJoyButtonDownHandler(HANDLE handle);
	void unregisterJoyButtonUpHandler(HANDLE handle);
	void unregisterJoyButtonPressHandler(HANDLE handle);

	/** Directly poll a key's value */
	inline bool isKeyDown(SDLKey key) const
	{
		return keyState[key];
	}

	/**
	Determines whether a key has been depressed.  If so, the value of the key
	is placed in 'key' and the function returns true.
	@param key The value of the depressed key as a SDK key code, else it will remain unmodified
	@param shift If the shift key was depressed, then this is set to false
	@return true if a key was depressed, false otherwise
	*/
	bool getKey(SDLKey &key, bool &shift);

private:
	/** Associates names with each keyboard key */
	void buildKeymap();

	/** Close all joysticks */
	void closeJoysticks();

	/** Open all joysticks */
	void openJoysticks();

	void handleKeyDownEvents(SDLKey key);
	void handleKeyUpEvents(SDLKey key);
	void handleKeyPressEvents(SDLKey key);
	void handleMouseMoveEvents(int x, int y, int xrel, int yrel);
	void handleMouseDownLeftEvents(int x, int y);
	void handleMouseDownRightEvents(int x, int y);
	void handleMouseUpLeftEvents(int x, int y);
	void handleMouseUpRightEvents(int x, int y);
	void handleJoyButtonDownEvents(int joystick, int button);
	void handleJoyButtonUpEvents(int joystick, int button);
	void handleJoyButtonPressEvents(int joystick, int button);
	void handleJoyAxisMotionEvents(int joystick, int axis, int value);

private:
	/** SDL_QUIT should be conveyed to the application through this flag */
	bool *quitFlag;

	/** key name -> keyboard key */
	map<string, SDLKey> keymap;

	/** Array of all keys where true indicates that the key is down */
	bool keyState[SDLK_LAST];

	/** Joystick threshold / deadzone */
	int threshold;

	/** Open joysticks */
	vector<Joystick> &joysticks;
	
	/** Assigns UIDs / names to event handlers */
	UniqueIdFactory<HANDLE> nameFactory;

	map<HANDLE, EventKeyDown> EventHandlersKeyDown[SDLK_LAST];
	map<HANDLE, EventKeyUp> EventHandlersKeyUp[SDLK_LAST];
	map<HANDLE, EventKeyPress> EventHandlersKeyPress[SDLK_LAST];

	map<HANDLE, EventMouseMove> EventHandlersMouseMove;
	map<HANDLE, EventMouseDownLeft> EventHandlersMouseDownLeft;
	map<HANDLE, EventMouseDownRight> EventHandlersMouseDownRight;
	map<HANDLE, EventMouseUpLeft> EventHandlersMouseUpLeft;
	map<HANDLE, EventMouseUpRight> EventHandlersMouseUpRight;

	map<HANDLE, EventJoyButtonDown> EventHandlersJoyButtonDown[MAX_JOYSTICK_BUTTONS][MAX_JOYSTICKS];
	map<HANDLE, EventJoyButtonUp> EventHandlersJoyButtonUp[MAX_JOYSTICK_BUTTONS][MAX_JOYSTICKS];
	map<HANDLE, EventJoyButtonPress> EventHandlersJoyButtonPress[MAX_JOYSTICK_BUTTONS][MAX_JOYSTICKS];
	map<HANDLE, EventJoyAxisMotion> EventHandlersJoyAxisMotion[MAX_JOYSTICK_AXII][MAX_JOYSTICKS];
};

#endif
