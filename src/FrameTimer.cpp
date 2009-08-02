#include <cmath>
#include <SDL/SDL.h>

#include "FrameTimer.h"

FrameTimer::FrameTimer()
: prevTicks(0),
  fps(0),
  length(0)
{
	prevTicks = getTicks();
}

void FrameTimer::update()
{
	const unsigned int now = getTicks();
	length = now - prevTicks;
	fps = (unsigned int)ceilf(1000.0f / length);
	prevTicks = now; // save for next time
}

unsigned int FrameTimer::getTicks()
{
	return (unsigned int)SDL_GetTicks();
}
