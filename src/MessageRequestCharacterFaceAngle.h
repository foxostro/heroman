#ifndef MESSAGE_REQUEST_CHARACTER_FACE_ANGLE_H
#define MESSAGE_REQUEST_CHARACTER_FACE_ANGLE_H

#include "BlackBoard.h"

/** Request that the character turn to a specific facing off of the X-axis */
class MessageRequestCharacterFaceAngle : public Message
{
public:
	/**
	Construct the message
	@param facingAngle Radians
	*/
	MessageRequestCharacterFaceAngle(float facingAngle)
	: Message("MessageRequestCharacterFaceAngle")
	{
		addField("facingAngle", facingAngle);
	}
};

/** Request that the character turn by some angle off the x axis */
class MessageRequestCharacterTurn : public Message
{
public:
	/**
	Construct the message
	@param facingAngle Radians
	*/
	MessageRequestCharacterTurn(float dFacingAngle)
		: Message("MessageRequestCharacterTurn")
	{
		addField("dFacingAngle", dFacingAngle);
	}
};

#endif
