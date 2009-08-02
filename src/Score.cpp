#include "stdafx.h"
#include "World.h"
#include "MessageChangeScore.h"
#include "Score.h"

/** Constructor. Sets the initial score. */
Score::Score(int initScore, BlackBoardUID uid, BlackBoard *parentBlackBoard)
	: BlackBoardSubscriber(uid, parentBlackBoard)
{
	score = initScore;
	trackLen = DEFAULT_TRACK_LEN;
	multiplier = 1.0;
	multTTL = 0;

	registerMessageHandler("MessageChangeScore",	bind(&Score::handleMessageChangeScore, this, _1));
}

/** Destructor */
Score::~Score(void)
{
}

/** Updates any necessary scoring data */
void Score::update(float milliseconds)
{
	multTTL -= (int)milliseconds;
	timeToComplete += milliseconds / 1000.0;

	//if TTL of multiplier is <= 0, reset multiplier
	if (multTTL <= 0 && multiplier != 1.0)
	{
		multiplier = 1.0;
	}
}

/** Returns the current score */
int Score::getScore()
{
	return score;
}

/** Sets the number of recent changes to track */
void Score::setTrackLen(unsigned int t)
{
	trackLen = t;
}

/** Sets the score multiplier and the time for which it is valid */
void Score::setMultiplier(float m, unsigned int t)
{
	multiplier = m;
	multTTL = t;
}

/** Returns the current score multiplier */
float Score::getMultiplier()
{
	return multiplier;
}

/** Records a change to the score */
void Score::recordChange(int delta, char op, unsigned int time)
{
	//evict oldest data from queue if length becomes too long
	while (deltas.size() >= trackLen)
		deltas.pop();

	//record change
	deltas.push(make_tuple(delta, op, time));
}

/** 
Callback handler for MessageChangeScore. Upon receipt of this
message, the score is changed by the value specified, using the given
operation. 
*/
void Score::handleMessageChangeScore(Message &m)
{
	int val = m.getField<int>("value");
	char op = m.getField<char>("operation");
	bool ignoreMod = m.getField<bool>("ignoreModifiers");

	if(!ignoreMod)
	{
		val = (int)((float)val * getMultiplier());
	}

	switch (op)
	{
	case '+':
		score += val;
		break;

	case '-':
		score -= val;
		break;

	case '*':
		score *= val;
		break;

	case '/':
		score /= val;
		break;

	default:
		break;
	}

	recordChange(val, op, SDL_GetTicks());
}
