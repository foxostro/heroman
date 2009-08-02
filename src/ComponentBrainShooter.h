#ifndef COMPONENT_BRAIN_SHOOTER_H
#define COMPONENT_BRAIN_SHOOTER_H

#include "ComponentBrain.h"

/**
Brain component implements some behavior for the actor.
Wanders randomly until it sees a player, then tries to close in and attack.
Attacks with whatever gun it has handy.
*/
class ComponentBrainShooter : public ComponentBrain
{
private:
	enum AiState
	{
		StateWander,
		StateChase
	};

public:
	virtual string getTypeString() const { return "Brain"; }

	ComponentBrainShooter(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws any visual component represented here */
	virtual void draw() const;

	/** Resets all object members to defaults */
	virtual void resetMembers();

private:
	void handleMessagePhysicsCollision(Message &);

	Action getNextAction(float milliseconds);
	Action getNextAction_wander(float milliseconds);
	Action getNextAction_chase(float milliseconds);

	/** Draw FOV for debug purposes */
	void drawFOV() const;

	/**
	Determines whether the Monster can detect some player within its FOV
	If a player was detected, it is recorded in the member 'targetPlayer'
	@return true if the player was detected
	*/
	bool canDetectAnyPlayer();

	/**
	Determines whether the Monster can detect the given player within its FOV
	@param player the given player
	@return true if the player was detected
	*/
	bool canDetectPlayer(const Actor &player);

	static bool isAngleWithinFOV(float angle, float minFOV, float maxFOV);

private:
	AiState state, nextState;
	Actor *targetPlayer;
	float fov;
	float maxSightDistance;
	float resumeWanderThreshold;
	float shootDistance;
	float wanderTimer;
	float wanderAngle;
};

#endif
