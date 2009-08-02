#ifndef COMPONENT_RENDER_AS_LAMP_H
#define COMPONENT_RENDER_AS_LAMP_H

#include "PropertyBag.h"

class World;

/** Renders the object as a wireframe switch */
class ComponentRenderAsLamp : public Component
{
public:
	virtual string getTypeString() const { return "RenderAsLamp"; }

	ComponentRenderAsLamp(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Updates the object */
	virtual void update(float milliseconds);

private:
	void handleMessagePositionHasBeenSet(Message &message);
	void handleMessageOrientationHasBeenSet(Message &message);
	void handleMessageRequestSetHeight(Message &message);
	void handleMessageClientUsesObject(Message &message);

	void drawCircle(float radius, float z) const;

private:
	vec3 lastReportedPosition;
	mat3 lastReportedOrientation;
	float lastReportedHeight;

	bool state;
	float lightRadius;
};

#endif
