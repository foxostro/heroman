#include "stdafx.h"
#include "gui.h"
#include "Widget.h"

Widget::Widget()
: visible(true),
  dead(false)
{}

Widget::~Widget()
{
	while(!children.empty())
	{
		delete(*children.begin());
		children.erase(children.begin());
	}
}

void Widget::addChild(Widget *widget)
{
	ASSERT(widget!=0, "widget was NULL");
	children.push_back(widget);
}

void Widget::renderTree(const vec2 &base) const
{
	if(!visible)
		return;

	absPos = base + relPos; // Update origins

	glPushMatrix();
	glTranslatef(relPos.x, relPos.y, 0.0f);

	draw();

    for(list<Widget*>::const_iterator i = children.begin();
		i != children.end();
		++i)
    {
        Widget *widget = *i;
        widget->renderTree(absPos);
    }

	glPopMatrix();
}

void Widget::processTree(float deltaTime)
{
	if(!visible)
		return;

	pruneDeadChildren();

	if(!children.empty())
	{
		// Process Children in reverse order of rendering
		list<Widget*> reversedList;
		transform(children.begin(), children.end(), back_inserter(reversedList), _1);

        for(list<Widget*>::const_iterator i = reversedList.begin(); i != reversedList.end(); ++i)
        {
            Widget *widget = *i;
            widget->processTree(deltaTime);
        }
	}

	update(deltaTime);
}

void Widget::pruneDeadChildren()
{
	list<Widget*>::iterator iter = children.begin();

	while(iter != children.end())
	{
		if((*iter)->dead)
		{
			delete(*iter);
			iter = children.erase(iter);
		}
		else
			iter++;
	}
}
