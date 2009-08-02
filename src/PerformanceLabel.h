#ifndef PERFORMANCE_LABEL_H
#define PERFORMANCE_LABEL_H

#include "FrameTimer.h"
#include "LabelWidget.h"

/** Displays performance statistics */
class PerformanceLabel : public LabelWidget
{
private:
	FrameTimer &frameTimer;

public:
	/**
	Constructor
	@param pos Position of the widget
	@param font Font in which to print the label text
	@param timer Calculates FPS and frame length
	*/
	PerformanceLabel(const vec2 &pos, shared_ptr<Font> font, FrameTimer &timer);

	/**
	Updates the widget
	@param deltaTime Milliseconds since the last tick
	*/
	virtual void update(float deltaTime);
};

#endif
