#ifndef _FRAME_TIMER_H_
#define _FRAME_TIMER_H_

/** Tracks time between frames and frame FPS stats */
class FrameTimer
{
	unsigned int prevTicks; /** Tick count of the last frame we were updated */
	unsigned int fps;       /** Most recent measurement of frames per second */
	unsigned int length;    /** Length of the last frame */

public:
	FrameTimer();

	/** Update frame counter */
	void update();

	/** get the time since the last call to update */
	inline unsigned int getElapsedTime() const
	{
		return getTicks() - prevTicks;
	}

	/** Get frames per second */
	inline unsigned int getFPS() const
	{
		return fps;
	}

	/** get the time of the last frame */
	inline unsigned int getLength() const
	{
		return length;
	}

private:
	/** Counts milliseconds since the app started */
	static unsigned int getTicks();
};

#endif
