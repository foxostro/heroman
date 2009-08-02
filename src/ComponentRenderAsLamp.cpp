#include "stdafx.h"
#include "color.h"
#include "World.h"
#include "Actor.h"
#include "ComponentRenderAsLamp.h"

ComponentRenderAsLamp::ComponentRenderAsLamp(BlackBoardUID uid,
												 BlackBoard *blackBoard)
												 : Component(uid, blackBoard)
{
	registerMessageHandler("MessagePositionHasBeenSet",    bind(&ComponentRenderAsLamp::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageRequestSetHeight",      bind(&ComponentRenderAsLamp::handleMessageRequestSetHeight, this, _1));
	registerMessageHandler("MessageOrientationHasBeenSet", bind(&ComponentRenderAsLamp::handleMessageOrientationHasBeenSet, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",    bind(&ComponentRenderAsLamp::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageClientUsesObject",      bind(&ComponentRenderAsLamp::handleMessageClientUsesObject, this, _1));
}

void ComponentRenderAsLamp::handleMessageOrientationHasBeenSet(Message &message)
{
	lastReportedOrientation = message.getField<mat3>("orientation");
}

void ComponentRenderAsLamp::handleMessagePositionHasBeenSet(Message &message)
{
	lastReportedPosition = message.getField<vec3>("position");
}

void ComponentRenderAsLamp::handleMessageRequestSetHeight(Message &message)
{
	lastReportedHeight = message.getField<float>("height");
}

void ComponentRenderAsLamp::handleMessageClientUsesObject(Message &message)
{
	state = !state;
}

void ComponentRenderAsLamp::resetMembers()
{
	lastReportedPosition.zero();
	lastReportedOrientation.identity();
	lastReportedHeight = 1.0f;

	state = false;
}

void ComponentRenderAsLamp::draw() const
{
	if(!state)
		return;

	CHECK_GL_ERROR();

	const mat4 transformation(lastReportedPosition,
	                          vec3(1.0f, 0.0f, 0.0f),
	                          vec3(0.0f, 1.0f, 0.0f),
	                          vec3(0.0f, 0.0f, 1.0f));

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glLineWidth(2.0f);
	glColor4fv(yellow);
	glPushMatrix();
	glMultMatrixf(transformation);
	drawCircle(lightRadius, 0.5f);
	glPopMatrix();
	glPopAttrib();

	CHECK_GL_ERROR();
}

void ComponentRenderAsLamp::drawCircle(float radius, float z) const
{
	CHECK_GL_ERROR();
	glPushMatrix();
	glScalef(radius, radius, 1.0f);
	glBegin(GL_LINE_LOOP);
	for(int i=0; i<16; ++i)
	{
		float angle = (float)(i * M_PI / 8.0);
		glVertex3f(sinf(angle), cosf(angle), z);
	}
	glEnd();
	glPopMatrix();
	CHECK_GL_ERROR();
}

void ComponentRenderAsLamp::load(const PropertyBag &data)
{
	resetMembers();

	const string initialState = data.getString("initialState");
	state = (toLowerCase(initialState) == "on");

	lightRadius = data.getFloat("lightRadius");
}

void ComponentRenderAsLamp::update(float milliseconds)
{}
