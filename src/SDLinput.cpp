#include "stdafx.h"
#include "SDLinput.h"

SDLinput::~SDLinput()
{}

SDLinput::SDLinput(vector<Joystick> &_joysticks)
: quitFlag(0),
  threshold(10000),
  joysticks(_joysticks),
  nameFactory(1000)
{
	for(size_t s = 0; s < sizeof(keyState); ++s)
	{
		keyState[s] = false;
	}

	buildKeymap();

	SDL_WM_GrabInput(SDL_GRAB_ON);
}

void SDLinput::handleKeyDownEvents(SDLKey key)
{
	for(map<HANDLE, EventKeyDown>::const_iterator i=EventHandlersKeyDown[key].begin();
		i!=EventHandlersKeyDown[key].end();
		++i)
	{
		(i->second)();
	}
}

void SDLinput::handleKeyUpEvents(SDLKey key)
{
	for(map<HANDLE, EventKeyUp>::const_iterator i=EventHandlersKeyUp[key].begin();
		i!=EventHandlersKeyUp[key].end();
		++i)
	{
		(i->second)();
	}
}

void SDLinput::handleKeyPressEvents(SDLKey key)
{
	for(map<HANDLE, EventKeyPress>::const_iterator i=EventHandlersKeyPress[key].begin();
		i!=EventHandlersKeyPress[key].end();
		++i)
	{
		(i->second)();
	}
}

void SDLinput::handleMouseMoveEvents(int x, int y, int xrel, int yrel)
{
	for(map<HANDLE, EventMouseMove>::const_iterator i=EventHandlersMouseMove.begin();
		i!=EventHandlersMouseMove.end();
		++i)
	{
		(i->second)(ivec2(x, y), ivec2(xrel, yrel));
	}
}

void SDLinput::handleMouseDownLeftEvents(int x, int y)
{
	for(map<HANDLE, EventMouseDownLeft>::const_iterator i=EventHandlersMouseDownLeft.begin(); i!=EventHandlersMouseDownLeft.end(); ++i)
	{
		(i->second)(x, y);
	}
}

void SDLinput::handleMouseDownRightEvents(int x, int y)
{
	for(map<HANDLE, EventMouseDownRight>::const_iterator i=EventHandlersMouseDownRight.begin(); i!=EventHandlersMouseDownRight.end(); ++i)
	{
		(i->second)(x, y);
	}
}

void SDLinput::handleMouseUpLeftEvents(int x, int y)
{
	for(map<HANDLE, EventMouseUpLeft>::const_iterator i=EventHandlersMouseUpLeft.begin(); i!=EventHandlersMouseUpLeft.end(); ++i)
	{
		(i->second)(x, y);
	}
}

void SDLinput::handleMouseUpRightEvents(int x, int y)
{
	for(map<HANDLE, EventMouseUpRight>::const_iterator i=EventHandlersMouseUpRight.begin(); i!=EventHandlersMouseUpRight.end(); ++i)
	{
		(i->second)(x, y);
	}
}

void SDLinput::handleJoyAxisMotionEvents(int joystick, int axis, int value)
{
	map<HANDLE, EventJoyAxisMotion> &events = EventHandlersJoyAxisMotion[axis][joystick];

	for(map<HANDLE, EventJoyAxisMotion>::const_iterator i=events.begin();
		i!=events.end();
		++i)
	{
		(i->second)(value);
	}
}

void SDLinput::handleJoyButtonDownEvents(int joystick, int button)
{
	map<HANDLE, EventJoyButtonDown> &events = EventHandlersJoyButtonDown[button][joystick];

	for(map<HANDLE, EventJoyButtonDown>::const_iterator i=events.begin();
		i!=events.end();
		++i)
	{
		(i->second)();
	}
}

void SDLinput::handleJoyButtonUpEvents(int joystick, int button)
{
	map<HANDLE, EventJoyButtonUp> &events = EventHandlersJoyButtonUp[button][joystick];

	for(map<HANDLE, EventJoyButtonUp>::const_iterator i=events.begin();
		i!=events.end();
		++i)
	{
		(i->second)();
	}
}

void SDLinput::handleJoyButtonPressEvents(int joystick, int button)
{
	map<HANDLE, EventJoyButtonUp> &events = EventHandlersJoyButtonPress[button][joystick];

	for(map<HANDLE, EventJoyButtonPress>::const_iterator i=events.begin();
		i!=events.end();
		++i)
	{
		(i->second)();
	}
}

bool SDLinput::getKey(SDLKey &key, bool &shift)
{
	for(size_t i=0; i<sizeof(keyState); ++i)
	{
		if(keyState[i])
		{
			shift = keyState[SDLK_LSHIFT] || keyState[SDLK_RSHIFT];
			key = (SDLKey)i;
			return true;
		}
	}

	return false;
}

void SDLinput::update()
{
	ASSERT(quitFlag!=0, "Member \'quitFlag\' is null");

	SDL_Event event;
	event.type = SDL_NOEVENT;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			keyState[event.key.keysym.sym] = event.key.state == SDL_PRESSED;
			handleKeyPressEvents(event.key.keysym.sym);
			break;

		case SDL_KEYUP:
			keyState[event.key.keysym.sym] = event.key.state == SDL_PRESSED;
			handleKeyUpEvents(event.key.keysym.sym);
			break;

		case SDL_MOUSEMOTION:
			handleMouseMoveEvents(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch(event.button.button)
			{
			case SDL_BUTTON_LEFT:
				handleMouseDownLeftEvents(event.motion.x, event.motion.y);
				break;

			case SDL_BUTTON_RIGHT:
				handleMouseDownRightEvents(event.motion.x, event.motion.y);
				break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch(event.button.button)
			{
			case SDL_BUTTON_LEFT:
				handleMouseUpLeftEvents(event.motion.x, event.motion.y);
				break;

			case SDL_BUTTON_RIGHT:
				handleMouseUpRightEvents(event.motion.x, event.motion.y);
				break;
			}
			break;

		case SDL_JOYBUTTONDOWN:
			joysticks[event.jbutton.which].btnState[event.jbutton.button] = event.jbutton.state==SDL_PRESSED;
			handleJoyButtonPressEvents(event.jbutton.which, event.jbutton.button);
			break;

		case SDL_JOYBUTTONUP:
			joysticks[event.jbutton.which].btnState[event.jbutton.button] = event.jbutton.state==SDL_PRESSED;
			handleJoyButtonUpEvents(event.jbutton.which, event.jbutton.button);
			break;

		case SDL_QUIT:
			(*quitFlag) = true;
			break;
		}
	}

	// Poll for joystick axis events
	for(vector<Joystick>::iterator i=joysticks.begin();
	    i!=joysticks.end();
	    ++i)
	{
		const Joystick &joystick = *i;

		if(joystick.handle)
		{
			const int numAxes = SDL_JoystickNumAxes(joystick.handle);
			for(int axis=0; axis<numAxes; ++axis)
			{
				Sint16 value = SDL_JoystickGetAxis(joystick.handle, axis);

				if(abs(value) > threshold)
				{
					handleJoyAxisMotionEvents(joystick.which, axis, value);
				}
			}
		}
	}

	// Fire off events for key holds
	for(size_t key=0; key<=sizeof(keyState); ++key)
	{
		if(keyState[key])
		{
			handleKeyDownEvents((SDLKey)key);
		}
	}

	// Fire off events for joystick button holds
	for(vector<Joystick>::iterator i=joysticks.begin();
		i!=joysticks.end();
		++i)
	{
		const Joystick &joystick = *i;

		for(int i=0; i<64; ++i)
		{
			if(joystick.btnState[i])
			{
				handleJoyButtonDownEvents(joystick.which, i);
			}
		}
	}

	//SDL_WarpMouse(640/2, 480/2);
}

SDLinput::HANDLE SDLinput::registerKeyDownHandler(SDLKey key, const EventKeyDown &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersKeyDown[key].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerKeyUpHandler(SDLKey key, const EventKeyUp &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersKeyUp[key].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerKeyPressHandler(SDLKey key, const EventKeyPress &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersKeyPress[key].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerJoyButtonDownHandler(int joystick, int button, const EventJoyButtonDown &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersJoyButtonDown[button][joystick].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerJoyButtonUpHandler(int joystick, int button, const EventJoyButtonUp &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersJoyButtonUp[button][joystick].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerJoyButtonPressHandler(int joystick, int button, const EventJoyButtonPress &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersJoyButtonPress[button][joystick].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerJoyAxisMotionHandler(int joystick, int axis, const EventJoyAxisMotion &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersJoyAxisMotion[axis][joystick].insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerMouseMoveHandler(const EventMouseMove &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersMouseMove.insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerMouseDownLeftHandler(const EventMouseDownLeft &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersMouseDownLeft.insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerMouseDownRightHandler(const EventMouseDownRight &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersMouseDownRight.insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerMouseUpLeftHandler(const EventMouseUpLeft &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersMouseUpLeft.insert(make_pair(name, handler));
	return name;
}

SDLinput::HANDLE SDLinput::registerMouseUpRightHandler(const EventMouseUpRight &handler)
{
	const HANDLE name = nameFactory.getUid();
	EventHandlersMouseUpRight.insert(make_pair(name, handler));
	return name;
}

void SDLinput::unregisterKeyDownHandler(HANDLE handle)
{
	for(int i=0; i<SDLK_LAST; ++i)
	{
		if(EventHandlersKeyDown[i].find(handle)
			!= EventHandlersKeyDown[i].end())
		{
			EventHandlersKeyDown[i].erase(handle);
		}
	}
}

void SDLinput::unregisterKeyUpHandler(HANDLE handle)
{
	for(int i=0; i<SDLK_LAST; ++i)
	{
		if(EventHandlersKeyUp[i].find(handle)
			!= EventHandlersKeyUp[i].end())
		{
			EventHandlersKeyUp[i].erase(handle);
		}
	}
}

void SDLinput::unregisterKeyPressHandler(HANDLE handle)
{
	for(int i=0; i<SDLK_LAST; ++i)
	{
		if(EventHandlersKeyPress[i].find(handle)
			!= EventHandlersKeyPress[i].end())
		{
			EventHandlersKeyPress[i].erase(handle);
		}
	}
}

void SDLinput::unregisterMouseMoveHandler(HANDLE handle)
{
	if(EventHandlersMouseMove.find(handle)
		!= EventHandlersMouseMove.end())
	{
		EventHandlersMouseMove.erase(handle);
	}
}

void SDLinput::unregisterMouseDownLeftHandler(HANDLE handle)
{
	if(EventHandlersMouseDownLeft.find(handle)
		!= EventHandlersMouseDownLeft.end())
	{
		EventHandlersMouseDownLeft.erase(handle);
	}
}

void SDLinput::unregisterMouseDownRightHandler(HANDLE handle)
{
	if(EventHandlersMouseDownRight.find(handle)
		!= EventHandlersMouseDownRight.end())
	{
		EventHandlersMouseDownRight.erase(handle);
	}
}

void SDLinput::unregisterMouseUpLeftHandler(HANDLE handle)
{
	if(EventHandlersMouseUpLeft.find(handle)
		!= EventHandlersMouseUpLeft.end())
	{
		EventHandlersMouseUpLeft.erase(handle);
	}
}

void SDLinput::unregisterMouseUpRightHandler(HANDLE handle)
{
	if(EventHandlersMouseUpRight.find(handle)
		!= EventHandlersMouseUpRight.end())
	{
		EventHandlersMouseUpRight.erase(handle);
	}
}

void SDLinput::unregisterJoyAxisMotionHandler(HANDLE handle)
{
	for(int i=0; i<MAX_JOYSTICK_AXII; ++i)
	{
		for(int j=0; j<MAX_JOYSTICKS; ++j)
		{
			if(EventHandlersJoyAxisMotion[i][j].find(handle)
				!= EventHandlersJoyAxisMotion[i][j].end())
			{
				EventHandlersJoyAxisMotion[i][j].erase(handle);
			}
		}
	}
}

void SDLinput::unregisterJoyButtonDownHandler(HANDLE handle)
{
	for(int i=0; i<MAX_JOYSTICK_BUTTONS; ++i)
	{
		for(int j=0; j<MAX_JOYSTICKS; ++j)
		{
			if(EventHandlersJoyButtonDown[i][j].find(handle)
				!= EventHandlersJoyButtonDown[i][j].end())
			{
				EventHandlersJoyButtonDown[i][j].erase(handle);
			}
		}
	}
}

void SDLinput::unregisterJoyButtonUpHandler(HANDLE handle)
{
	for(int i=0; i<MAX_JOYSTICK_BUTTONS; ++i)
	{
		for(int j=0; j<MAX_JOYSTICKS; ++j)
		{
			if(EventHandlersJoyButtonUp[i][j].find(handle)
				!= EventHandlersJoyButtonUp[i][j].end())
			{
				EventHandlersJoyButtonUp[i][j].erase(handle);
			}
		}
	}
}

void SDLinput::unregisterJoyButtonPressHandler(HANDLE handle)
{
	for(int i=0; i<MAX_JOYSTICK_BUTTONS; ++i)
	{
		for(int j=0; j<MAX_JOYSTICKS; ++j)
		{
			if(EventHandlersJoyButtonPress[i][j].find(handle)
				!= EventHandlersJoyButtonPress[i][j].end())
			{
				EventHandlersJoyButtonPress[i][j].erase(handle);
			}
		}
	}
}

void SDLinput::buildKeymap()
{
	keymap["Backspace"] = SDLK_BACKSPACE;
	keymap["Tab"] = SDLK_TAB;
	keymap["Clear"] = SDLK_CLEAR;
	keymap["Return"] = SDLK_RETURN;
	keymap["L_Shift"] = SDLK_LSHIFT;
	keymap["R_Shift"] = SDLK_RSHIFT;
	keymap["L_Control"] = SDLK_LCTRL;
	keymap["R_Control"] = SDLK_RCTRL;
	keymap["Pause"] = SDLK_BREAK;
	keymap["Escape"] = SDLK_ESCAPE;
	keymap["Space"] = SDLK_SPACE;
	keymap["Page_Up"] = SDLK_PAGEUP;
	keymap["Page_Down"] = SDLK_PAGEDOWN;
	keymap["End"] = SDLK_END;
	keymap["Home"] = SDLK_HOME;
	keymap["Left"] = SDLK_LEFT;
	keymap["Up"] = SDLK_UP;
	keymap["Right"] = SDLK_RIGHT;
	keymap["Down"] = SDLK_DOWN;
	keymap["Insert"] = SDLK_INSERT;
	keymap["Delete"] = SDLK_DELETE;
	keymap["Numpad_0"] = SDLK_KP0;
	keymap["Numpad_1"] = SDLK_KP1;
	keymap["Numpad_2"] = SDLK_KP2;
	keymap["Numpad_3"] = SDLK_KP3;
	keymap["Numpad_4"] = SDLK_KP4;
	keymap["Numpad_5"] = SDLK_KP5;
	keymap["Numpad_6"] = SDLK_KP6;
	keymap["Numpad_7"] = SDLK_KP7;
	keymap["Numpad_8"] = SDLK_KP8;
	keymap["Numpad_9"] = SDLK_KP9;
	keymap["Numpad_Multiply"] = SDLK_KP_MULTIPLY;
	keymap["Numpad_Add"] = SDLK_KP_PLUS;
	keymap["Numpad_Subtract"] = SDLK_KP_MINUS;
	keymap["Numpad_Dot"] = SDLK_KP_PERIOD;
	keymap["Numpad_Divide"] = SDLK_KP_DIVIDE;
	keymap["Numpad_Enter"] = SDLK_KP_ENTER;
	keymap["F1"] = SDLK_F1;
	keymap["F2"] = SDLK_F2;
	keymap["F3"] = SDLK_F3;
	keymap["F4"] = SDLK_F4;
	keymap["F5"] = SDLK_F5;
	keymap["F6"] = SDLK_F6;
	keymap["F7"] = SDLK_F7;
	keymap["F8"] = SDLK_F8;
	keymap["F9"] = SDLK_F9;
	keymap["F10"] = SDLK_F10;
	keymap["F11"] = SDLK_F11;
	keymap["F12"] = SDLK_F12;
	keymap["F13"] = SDLK_F13;
	keymap["F14"] = SDLK_F14;
	keymap["F15"] = SDLK_F15;

	for(char c = 'a'; c <= 'z'; ++c) //alphabetic keys
	{
		SDLKey key = (SDLKey)(   (int)SDLK_a + (c-'a')   );

		char name[] = {c, 0};

		keymap[name] = key;
	}

	for(char c = '0'; c <= '9'; ++c) //numeric keys
	{
		SDLKey key = (SDLKey)(   (int)SDLK_0 + (c-'0')   );

		char name[] = {c, 0};

		keymap[name] = key;
	}

	keymap["Comma"] = SDLK_COMMA;
	keymap["Period"] = SDLK_PERIOD;
	keymap["Slash"] = SDLK_SLASH;
	keymap["Semicolon"] = SDLK_SEMICOLON;
	keymap["Quote"] = SDLK_QUOTE;
	keymap["L_Bracket"] = SDLK_LEFTBRACKET;
	keymap["R_Bracket"] = SDLK_RIGHTBRACKET;
	keymap["Backslash"] = SDLK_BACKSLASH;
	keymap["Underscore"] = SDLK_UNDERSCORE;
	keymap["Equals"] = SDLK_EQUALS;
}
