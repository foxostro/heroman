#include "stdafx.h"

#include "Effect.h"
#include "LinearInterpolator.h"
#include "SDLinput.h"
#include "GameStateMachine.h"
#include "GameStateRun.h"
#include "MessagePlaySound.h"
#include "Menu.h"

Menu::Menu(vector<Joystick> &joysticks,
		   Kernel &_kernel,
		   GameStateMachine &_gameStateMachine,
		   GameStateRun &_stateRun,
		   BlackBoard &_blackBoard,
		   shared_ptr<Font> _font,
		   bool &_quitFlag)
: GameState(_kernel, _stateRun),
  selectedIndex(0),
  font(_font),
  blackBoard(_blackBoard),
  gameStateMachine(_gameStateMachine),
  input(joysticks)
{
	input.setApplicationQuitFlag(&_quitFlag);

	input.registerKeyPressHandler(SDLK_UP,     bind(&Menu::keypressMenuUp, this));
	input.registerKeyPressHandler(SDLK_DOWN,   bind(&Menu::keypressMenuDown, this));
	input.registerKeyPressHandler(SDLK_SPACE,  bind(&Menu::keypressMenuEnter, this));
	input.registerKeyPressHandler(SDLK_ESCAPE, bind(&GameStateMachine::popStateOrRevert, &gameStateMachine));
}

Menu::~Menu()
{}

void Menu::draw() const
{
	ASSERT(font, "Null pointer: font");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GameState::draw();

	effect_Begin(effect_GUI);

	for(size_t i=0; i<elements.size(); ++i)
	{
		const Element &item = elements[i];
		const string &label = item.getLabel();
		const bool enabled = item.isEnabled();
		const bool selected = (size_t)selectedIndex==i;
		const color c = (selected ? yellow : white) * (enabled ? 1.0f : 0.7f);

		glPushMatrix();
		glTranslatef(item.getPosition().x, item.getPosition().y, 0.1f);
		font->write(label, c, Font::SizeHuge);
		glPopMatrix();
	}

	effect_End();
}

void Menu::keypressMenuUp()
{
	selectedIndex = (--selectedIndex) < 0 ? (signed int)elements.size()-1 : selectedIndex;

	playMenuUpSound();
}

void Menu::keypressMenuDown()
{
	selectedIndex = (++selectedIndex) >= (signed int)elements.size() ? 0 : selectedIndex;

	playMenuDownSound();
}

void Menu::keypressMenuEnter()
{
	if(elements[selectedIndex].isEnabled())
	{
		activateElement(selectedIndex);
	}
	else
	{
		playMenuDisabledSound();
	}
}

void Menu::update(float)
{
	input.update();
	draw();
}

void Menu::playSound(const FileName &sound)
{
	blackBoard.relayMessage(MessagePlaySound(sound));
}

void Menu::playMenuUpSound()
{
	playSound(FileName("data/sound/up.wav"));
}

void Menu::playMenuDownSound()
{
	playSound(FileName("data/sound/down.wav"));
}

void Menu::playMenuActivateSound()
{
	playSound(FileName("data/sound/activate.wav"));
}

void Menu::playMenuDisabledSound()
{
	playSound(FileName("data/sound/disabled.wav"));
}
