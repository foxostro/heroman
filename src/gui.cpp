#include "stdafx.h"
#include "Effect.h"
#include "gui.h"

GUI::GUI()
{}

void GUI::addWidget(Widget *widget)
{
	root.addChild(widget);
}

void GUI::draw()
{
	effect_Begin(effect_GUI);
	root.renderTree(vec2(0,0));
	effect_End();
}

void GUI::update(float deltaTime)
{
	root.processTree(deltaTime);
}
