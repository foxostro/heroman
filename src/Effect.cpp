#include "stdafx.h"
#include "color.h"
#include "Effect.h"

EFFECT currentEffect = effect_NONE;

static void effect_GUI_Begin(void)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);

	// Cleanly cut away transparent areas
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.10f);

	// smooth the edges and show through to the background
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);

	// Set up the projection matrix for 2D
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 1024.0f, 0.0f, 768.0f, -1.0f, 1.0f);

	// Setup the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -0.2f);
}

static void effect_PARTICLE_Begin(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	glDepthMask(GL_FALSE);
}

static void effect_TRANSPARENT_Begin(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_FOG);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4fv(white);
}

static void effect_LIT_AND_TEXTURED_Begin(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void effect_UNLIT_AND_TEXTURED_Begin(void)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void effect_LIT_AND_UNTEXTURED_Begin(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

static void effect_UNLIT_AND_UNTEXTURED_Begin(void)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
}

void effect_Begin(EFFECT effect)
{
	ASSERT(currentEffect == effect_NONE,
		"cannot bind an effect while another is active");

	ASSERT(effect != effect_NONE,
		"effect_NONE is not an assignable effect");

	CHECK_GL_ERROR();

	// Save settings
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glColor4fv(white);
	
	// Save the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Save the model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	CHECK_GL_ERROR();

	switch(currentEffect = effect)
	{
	case effect_NONE:
		ERR("effect_NONE is not an assignable effect");
		break;

	case effect_GUI:
		effect_GUI_Begin();
		break;

	case effect_PARTICLE:
		effect_PARTICLE_Begin();
		break;

	case effect_TRANSPARENT:
		effect_TRANSPARENT_Begin();
		break;

	case effect_LIT_AND_TEXTURED:
		effect_LIT_AND_TEXTURED_Begin();
		break;

	case effect_UNLIT_AND_TEXTURED:
		effect_UNLIT_AND_TEXTURED_Begin();
		break;

	case effect_LIT_AND_UNTEXTURED:
		effect_LIT_AND_UNTEXTURED_Begin();
		break;

	case effect_UNLIT_AND_UNTEXTURED: 
		effect_UNLIT_AND_UNTEXTURED_Begin();
		break;
	};
}

void effect_End(void)
{
	ASSERT(currentEffect != effect_NONE,
		"cannot end an effect without first starting one");

	CHECK_GL_ERROR();

	// Disable arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Restore the model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// Restore settings
	glPopAttrib();

	CHECK_GL_ERROR();

	currentEffect = effect_NONE;
}
