#include "stdafx.h"
#include "AnimationControllerFactory.h"
#include "Actor.h"
#include "MessagePositionHasBeenSet.h"
#include "MessageSetModel.h"
#include "MessageChangeAnimation.h"
#include "MessageRequestSetHeight.h"
#include "ComponentDeathBehavior.h"
#include "ComponentRenderAsModel.h"

extern shared_ptr<AnimationControllerFactory> g_ModelFactory;

ComponentRenderAsModel::ComponentRenderAsModel(BlackBoardUID _uid, BlackBoard *_blackBoard)
: Component(_uid, _blackBoard),
  model(0),
  dead(false),
  highlightMode(HighlightDisable),
  highlightIntensity(1.0f),
  hightlightIntensityRate(100.0f)
{
	registerMessageHandler("MessageCharacterHasBeenResurrected", bind(&ComponentRenderAsModel::handleMessageCharacterHasBeenResurrected, this, _1));
	registerMessageHandler("MessageCharacterHasDied",            bind(&ComponentRenderAsModel::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageRequestSetModel",             bind(&ComponentRenderAsModel::handleMessageRequestSetModel, this, _1));
	registerMessageHandler("MessageChangeAnimation",             bind(&ComponentRenderAsModel::handleMessageChangeAnimation, this, _1));
	registerMessageHandler("MessageRequestSetHeight",            bind(&ComponentRenderAsModel::handleMessageRequestSetHeight, this, _1));
	registerMessageHandler("MessageOrientationHasBeenSet",       bind(&ComponentRenderAsModel::handleMessageOrientationHasBeenSet, this, _1));
	registerMessageHandler("MessageRequestLookAt",               bind(&ComponentRenderAsModel::handleMessageRequestLookAt, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",          bind(&ComponentRenderAsModel::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageEnableModelHighlight",        bind(&ComponentRenderAsModel::handleMessageEnableModelHighlight, this, _1));
	registerMessageHandler("MessageDisableModelHighlight",       bind(&ComponentRenderAsModel::handleMessageDisableModelHighlight, this, _1));
}

void ComponentRenderAsModel::resetMembers()
{
	dead = false;
	lastReportedPosition.zero();
	lastReportedOrientation.identity();
	lastReportedHeight = 1.0f;
	modelHeight = 1.0f;
	independentModelOrientation = false;
	highlightMode = HighlightDisable;
	highlightIntensity = 1.0f;
	hightlightIntensityRate = 400.0f;
	outlineWidth = 2.0f;
	highlightTimeRemaining = 0.0f;
	colora = yellow;
	colorb = white;
}

void ComponentRenderAsModel::loadModel(const FileName &fileName)
{
	ASSERT(g_ModelFactory, "modelFactory is null");
	model = g_ModelFactory->createFromFile(fileName);

	// Calculate height of unscaled model
	modelHeight = model->calculateHeight();

	// default animation
	changeAnimation("idle");
}

void ComponentRenderAsModel::update(float milliseconds)
{
	ASSERT(model, "Null pointer: model");

	model->update(milliseconds);

	if(highlightMode != HighlightDisable)
	{
		// update highlighting intensity
		highlightIntensity += milliseconds/hightlightIntensityRate;
		highlightIntensity = (highlightIntensity>1.0f) ? 0.0f
		                                               : highlightIntensity;

		// Allow highlight to end after a period of time
		highlightTimeRemaining -= milliseconds;

		if(highlightTimeRemaining <= 0.0f)
		{
			highlightTimeRemaining = 0.0f;
			highlightMode = HighlightDisable;
		}
	}
}

void ComponentRenderAsModel::draw() const
{
	if(!dead || getDeathBehavior()!=Ghost)
	{
		ASSERT(model, "Null pointer: model");

		const float modelScale = lastReportedHeight / modelHeight;

		CHECK_GL_ERROR();

		mat4 transformation(lastReportedPosition,
		                    lastReportedOrientation.getAxisX(),
		                    lastReportedOrientation.getAxisY(),
		                    lastReportedOrientation.getAxisZ());

		glPushMatrix();
		glMultMatrixf(transformation);
		glTranslatef(0.0f, 0.0f, -lastReportedHeight/2.0f);
		glScalef(modelScale, modelScale, modelScale);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		drawModel(true);
		glPopAttrib();
		glPopMatrix();

		CHECK_GL_ERROR();
	}
}

void ComponentRenderAsModel::drawTransparentBits() const
{
	if(dead && getDeathBehavior()==Ghost)
	{
		ASSERT(model, "Null model");

		const float modelScale = lastReportedHeight / modelHeight;

		CHECK_GL_ERROR();

		mat4 transformation(lastReportedPosition,
			lastReportedOrientation.getAxisX(),
			lastReportedOrientation.getAxisY(),
			lastReportedOrientation.getAxisZ());

		glPushMatrix();
		glMultMatrixf(transformation);
		glTranslatef(0.0f, 0.0f, -lastReportedHeight/2.0f);
		glScalef(modelScale, modelScale, modelScale);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		drawGhostModel();
		glPopAttrib();
		glPopMatrix();

		CHECK_GL_ERROR();
	}
}

void ComponentRenderAsModel::drawFlash() const
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glColor4fv(color::interpolate(colora, colorb, highlightIntensity));
	model->draw(false);
}

void ComponentRenderAsModel::drawOutline() const
{
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	/*
	Now set the stencil to create a 2d mask of the object
	Increase the value in the stencil buffer if the depth test passes
	ie, if it's visible
	*/
	glStencilFunc(GL_ALWAYS, 1, 0xffff); // always passes
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);

	model->draw(true);

	/*
	At this point we have a mask of values > 0 where the object is
	so we set the stencil func to allow fragments where stencil >= 1
	*/
	glStencilFunc(GL_GEQUAL, 1, 0xffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // don't modify

	// draw wire frame outline
	glLineWidth(outlineWidth);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// yellow without any effects
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glColor4fv(color::interpolate(colora, colorb, highlightIntensity));

	model->draw(false);
}


bool ComponentRenderAsModel::changeAnimation(const string &name)
{
	ASSERT(model, "Null pointer: model");
	return model->requestAnimationChange(name);
}

void ComponentRenderAsModel::handleMessageRequestSetModel(Message &message)
{
	loadModel(message.getField<FileName>("fileName"));
}

void ComponentRenderAsModel::handleMessageRequestSetHeight(Message &message)
{
	lastReportedHeight = message.getField<float>("height");
}

void ComponentRenderAsModel::handleMessageOrientationHasBeenSet(Message &m)
{
    if(!independentModelOrientation)
    {
        lastReportedOrientation = m.getField<mat3>("orientation");
    }
}

void ComponentRenderAsModel::handleMessagePositionHasBeenSet(Message &message)
{
	lastReportedPosition = message.getField<vec3>("position");
}

void ComponentRenderAsModel::handleMessageChangeAnimation(Message &message)
{
	changeAnimation(message.getField<string>("animationName"));
}

void ComponentRenderAsModel::handleMessageRequestLookAt(Message &m)
{
    if(independentModelOrientation)
    {
        const vec3 lookDirection = m.getField<vec3>("lookDirection");
        
        const vec3 eye = lastReportedPosition;
        const vec3 center = eye + lookDirection;
        const vec3 up(0,0,1);
        
        mat3 orientation;
        orientation.lookAt(eye, center, up);
        lastReportedOrientation = orientation;
	}
}

void ComponentRenderAsModel::load(const PropertyBag &data)
{
	resetMembers();
	const FileName modelFileName = data.getFileName("model");
	loadModel(modelFileName);
	data.get("independentModelOrientation", independentModelOrientation);
}

void ComponentRenderAsModel::handleMessageCharacterHasBeenResurrected(Message&)
{
	dead = false;
}

void ComponentRenderAsModel::handleMessageCharacterHasDied(Message &message)
{
	dead = true;
}

DeathBehavior ComponentRenderAsModel::getDeathBehavior() const
{
	DeathBehavior db = Disappear;
	ActorPtr actor = getActorPtr();

	const ComponentDeathBehavior* c = dynamic_cast<const ComponentDeathBehavior*>(actor->getComponent("DeathBehavior"));

	if(c!=0)
	{
		db = c->getDeathBehavior();
	}

	return db;
}

void ComponentRenderAsModel::handleMessageEnableModelHighlight(Message &m)
{
	highlightMode = m.getField<HighlightMode>("mode");
	highlightTimeRemaining = m.getField<float>("time");
	colora = m.getField<color>("colora");
	colorb = m.getField<color>("colorb");
}

void ComponentRenderAsModel::handleMessageDisableModelHighlight(Message &)
{
	highlightMode = HighlightDisable;
}

void ComponentRenderAsModel::drawGhostModel() const
{
	static const color ghost_fill(0.9f, 0.9f, 1.0f, 0.15f);
	static const color ghost_outline(0.2f, 0.2f, 0.4f, 1.0f);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	/*
	Now set the stencil to create a 2d mask of the object
	Increase the value in the stencil buffer if the depth test passes
	ie, if it's visible
	*/
	glStencilFunc(GL_ALWAYS, 1, 0xffff); // always passes
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	glColorMask(0, 0, 0, 0);
	model->draw(false); // z-pass should be fast on modern hardware
	glColorMask(1, 1, 1, 1);

	// ghost fill
	glStencilFunc(GL_LESS, 1, 0xffff);
	glColor4fv(ghost_fill);
	model->draw(false);

	// ghost outline
	glStencilFunc(GL_GEQUAL, 1, 0xffff);
	glLineWidth(outlineWidth*2.0f);
	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor4fv(ghost_outline);
	model->draw(false);
}
