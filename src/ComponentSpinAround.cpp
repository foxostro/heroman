#include "stdafx.h"
#include "MessageRequestPositionBeSet.h"
#include "MessageRequestLookAt.h"
#include "ComponentSpinAround.h"

ComponentSpinAround::ComponentSpinAround(BlackBoardUID uid,
										 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	resetMembers();
	registerMessageHandler("MessageDeclareInitialPosition", bind(&ComponentSpinAround::handleMessageDeclareInitialPosition, this, _1));
}

void ComponentSpinAround::resetMembers()
{
	facingAngle = 0.0f;
	rotationSpeed = (float)(2.0 * M_PI) / 1000.0f;
	bounce = 0.0f;
	bounceHeight = 0.5f;
	bounceSpeed = (float)(2.0 * M_PI) / 1000.0f;
}

void ComponentSpinAround::load(const PropertyBag &data)
{
	resetMembers();
	rotationSpeed = data.getFloat("rotationSpeed");
	bounceHeight = data.getFloat("bounceHeight");
	bounceSpeed = data.getFloat("bounceSpeed");
}

void ComponentSpinAround::update(float milliseconds)
{
	facingAngle = angle_clamp(facingAngle += rotationSpeed*milliseconds);
	bounce = angle_clamp(bounce += bounceSpeed*milliseconds);

	mat3 orientation = mat3::fromRotateZ(facingAngle);
	orientation = orientation * mat3::fromRotateX((float)(M_PI / 2.0));
	getParentBlackBoard().relayMessage(MessageRequestSetOrientation(orientation));

	vec3 offset = vec3(0.0f, 0.0f, cosf(bounce))*bounceHeight;
	vec3 pos = initialPosition + offset;
	getParentBlackBoard().relayMessage(MessageRequestPositionBeSet(pos));
}

void ComponentSpinAround::handleMessageDeclareInitialPosition(Message &m)
{
	initialPosition = m.getField<vec3>("position");
}
