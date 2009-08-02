#include "stdafx.h"
#include "PerformanceLabel.h"

PerformanceLabel::PerformanceLabel(const vec2 &pos,
								   shared_ptr<Font> font,
								   FrameTimer &timer)
: LabelWidget("---", pos, white, Font::SizeHuge, font),
  frameTimer(timer)
{
	visible = true;
}

void PerformanceLabel::update(float)
{
	setLabel(string("FPS: ") + sizet_to_string(frameTimer.getFPS()));
}
