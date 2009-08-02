#include "stdafx.h"
#include "PhysicsEngine.h"
#include "ComponentPhysics.h"
#include "ComponentRenderAsPhysicsGeom.h"

ComponentRenderAsPhysicsGeom::
ComponentRenderAsPhysicsGeom(BlackBoardUID _uid, BlackBoard *_blackBoard)
: Component(_uid, _blackBoard)
{}

void ComponentRenderAsPhysicsGeom::resetMembers()
{}

void ComponentRenderAsPhysicsGeom::update(float milliseconds)
{}

void ComponentRenderAsPhysicsGeom::draw() const
{
	CHECK_GL_ERROR();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);

		// Draw the physics engine geometry for this object
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor4fv(white);
		dGeomID geom = getPrerequisiteComponent<ComponentPhysics>("Physics").getGeom();
		PhysicsEngine::drawGeom(geom);

		// Draw the AABB if we are displaying verbose debug information
		if(displayDebugData)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			PhysicsEngine::drawGeomAABB(geom);
		}
	}
    glPopAttrib();

	CHECK_GL_ERROR();
}

void ComponentRenderAsPhysicsGeom::load(const PropertyBag &data)
{}
