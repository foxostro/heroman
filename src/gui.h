#ifndef _GUI_H_
#define _GUI_H_

#include "Widget.h"

/**
Owns the hierarchy of widgets.  Updates and renders the tree each tick,
and passes events to appropriate widgets.
*/
class GUI
{
public:
	GUI();

	/** Draws all widgets */
	void draw();

	/**
	Updates all widgets each tick
	@param deltaTime the milliseconds since the last update
	*/
	void update(float deltaTime);

	/**
	Adds a widget to the GUI at the root level
	@param widget Widget to add to the GUI
	*/
	void addWidget(Widget *widget);

private:
	Widget root;
};

#endif
