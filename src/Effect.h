#ifndef _EFFECT_H_
#define _EFFECT_H_

enum EFFECT
{
	effect_NONE,
	effect_GUI,
	effect_PARTICLE,
	effect_TRANSPARENT,
	effect_LIT_AND_TEXTURED,
	effect_UNLIT_AND_TEXTURED,
	effect_LIT_AND_UNTEXTURED,
	effect_UNLIT_AND_UNTEXTURED
};

/**
Binds an effect to use when rendering.  It is an error to begin
an effect while one has not yet been ended.
@param effect Name of the effect to begin using
*/
void effect_Begin(EFFECT effect);

/**
Stops using the currently bound Effect.
It is an error to end an effect while one is not bound.
*/
void effect_End();

#endif
