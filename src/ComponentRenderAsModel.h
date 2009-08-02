#ifndef COMPONENT_RENDER_AS_MODEL_H
#define COMPONENT_RENDER_AS_MODEL_H

#include "PropertyBag.h"
#include "AnimationController.h"
#include "DeathBehavior.h"
#include "MessageHighlightModel.h"
#include "Component.h"

class ComponentRenderAsModel : public Component
{
public:
	virtual string getTypeString() const { return "RenderAsModel"; }

	ComponentRenderAsModel(BlackBoardUID _uid, BlackBoard *_blackBoard);

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Draws any transparent visual component represented here */
	virtual void drawTransparentBits() const;

	/** Updates the object */
	virtual void update(float milliseconds);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/**
	Loads a model for the object
	@param fileName The file name of a model's XML definition
	*/
	void loadModel(const FileName &fileName);

	/**
	Requests that the animation be changed to the specified animation at the next available opportunity
	@param name The name of the new animation
	*/
	bool changeAnimation(const string &name);

	const AnimationController &getModel() const
	{
		ASSERT(model, "Actor does not have model!");
		return *model;
	}

private:
	void handleMessageCharacterHasBeenResurrected(Message &message);
	void handleMessageCharacterHasDied(Message &message);
	void handleMessageRequestSetModel(Message &message);
	void handleMessageChangeAnimation(Message &message);
	void handleMessageRequestSetHeight(Message &message);
	void handleMessageOrientationHasBeenSet(Message &message);
	void handleMessageRequestLookAt(Message &message);
	void handleMessagePositionHasBeenSet(Message &message);
	void handleMessageEnableModelHighlight(Message &message);
	void handleMessageDisableModelHighlight(Message &message);

	/** Gets the death behavior of the character */
	DeathBehavior getDeathBehavior() const;

	/** Draws the outlined model */
	void drawOutline() const;

	/** Draws the flashing model */
	void drawFlash() const;

	/** Draws the model, possibly with a highlight */
	inline void drawModel(bool useMaterial) const
	{
		switch(highlightMode)
		{
		case HighlightFlash: drawFlash(); break;
		case HighlightOutline: drawOutline(); break;
		default: model->draw(useMaterial); break;
		}
	}

	/** Draws the transparent ghost model with the ghost material */
	void drawGhostModel() const;

private:
	AnimationController *model;
	vec3 lastReportedPosition;
	mat3 lastReportedOrientation;
	float lastReportedHeight;
	float modelHeight;
	bool independentModelOrientation;
	bool dead;

	HighlightMode highlightMode;
	float highlightIntensity;
	float hightlightIntensityRate;
	float outlineWidth;
	float highlightTimeRemaining;
	color colora;
	color colorb;
};

#endif
