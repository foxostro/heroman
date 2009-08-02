#ifndef _WIDGET_H_
#define _WIDGET_H_

/**
Base class for any GUI object that appears on screen.
@todo Input events (e.g. MouseDown, MouseEnter, MouseClick, etc)
@todo Mechanism to specify widget Z-Level
*/
class Widget
{
public:
	virtual ~Widget();
	Widget();

	/**
	Adds a child widget
	@param widget Ownership of the given widget passes to this object
	*/
	void addChild(Widget *widget);

	/**
	Renders the widget and its children
	@param base Position of the parent
	*/
	void renderTree(const vec2 &base) const;

	/**
	Updates the widget and its children
	@param deltaTime Millseconds since the last update
	*/
	void processTree(float deltaTime);

	/**
	Sets the position of the widget in relative coords
	@return Y
	*/
	virtual void setRelativePos(const vec2 &pos)
	{
		relPos = pos;
	}

	/**
	Sets the position of the widget in relative coords
	@return Y
	*/
	inline void setRelativePos(float x, float y)
	{
		setRelativePos(vec2(x,y));
	}

	/**
	Sets the width of the widget
	@param w The new width
	*/
	virtual void setWidth(float w)
	{
		dim.x = w;
	}

	/**
	Sets the height of the widget
	@param h The new height
	*/
	virtual void setHeight(float h)
	{
		dim.y = h;
	}

protected:
	/** Draws the widget */
	virtual void draw() const {}

	/**
	Updates the widget
	@param deltaTime Milliseconds since the last tick
	*/
	virtual void update(float deltaTime) {}

private:
	/** Removes and deletes all dead children */
	virtual void pruneDeadChildren();

public:
	/** Indicate that the widget is visible */
	bool visible;

	/**
	Indicates to the parent widget that they are free to delete this widget
	*/
	bool dead;

private:
	/** Position of the widget relative to the parent */
	vec2 relPos;

	/** Position of the widget relative to the window */
	mutable vec2 absPos;

	/** Dimensions of the widget */
	vec2 dim;

	/** Widgets can be nested and have child controls */
	list<Widget*> children;
};

#endif
