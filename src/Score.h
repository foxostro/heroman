#ifndef SCORE_H
#define SCORE_H

#include "BlackBoard.h"

#define DEFAULT_TRACK_LEN 5		/** the default number of changes to the score to track */

/** 
A class describing an in-game score value and associated values and
funtions.
*/
class Score : public BlackBoardSubscriber
{
public:

	/* -- Functions -- */
	/** Constructors */
	Score(int initScore, BlackBoardUID uid, BlackBoard *parentBlackBoard);
	Score(BlackBoardUID uid, BlackBoard *parentBlackBoard);

	/** Destructor */
	~Score(void);

	/**
	Updates any necessary scoring data.
	@param milliseconds the time since the last update
	*/
	void update(float milliseconds);

	/** Returns the current score */
	int getScore();

	/** 
	Sets the number of recent changes to track (default is 5) 
	@param t the number of changes to track
	*/
	void setTrackLen(unsigned int t);

	/**
	Sets the amount by which new changes to the score are multiplied.
	@param m the amount by which to multiply future scores
	@param t the time for which the multiplier is valid (0 for unlimited)
	*/
	void setMultiplier(float m, unsigned int t);

	/**
	Returns the current score multiplier.
	@return the current score multiplier
	*/
	float getMultiplier();

private:
	/* -- Functions -- */
	/** Callback handlers for messages */
	void handleMessageChangeScore(Message &m);

	/** 
	Records a change in the score.
	@param delta the numeric change
	@param op a character specifying the operation performed
	@param time the time the change was made
	*/
	void recordChange(int delta, char op, unsigned int time);

	/* -- Instance Variables -- */
	int score;					    /** the integral score */
	unsigned int trackLen;			/** the number of recent score changes to track */
	float multiplier;				/** the amount by which changes to the score are currently multiplied (only the + operation is supported */
	unsigned int multTTL;			/** the amount of time the multiplier is valid for (in milliseconds) */

	//miscellaneous stats
	unsigned int numMonstersKilled;	/** the number of monsters killed */
	unsigned int numCoinsCollected;	/** the number of coins collected */
	double timeToComplete;			/** the time taken to complete the current level (in seconds) */

	queue<tuple<int, char, unsigned int> > deltas;		/** the last few changes to the score 
																	(the number of changes is specified by trackLen).
																	format is: (value, operation, time) */
};

#endif
