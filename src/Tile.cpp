#include "stdafx.h"
#include "Material.h"
#include "Map.h"
#include "Tile.h"

void Tile::texCoord(float u, float v)
{
	glTexCoord2f(u, v);
	//glMultiTexCoord2f(GL_TEXTURE0, u, v);
}

Tile::Tile()
{
	setupBoundingBox(1,1);
}

void Tile::create(float width,
				  float height,
				  const Material &_floorMat,
				  const Material &_wallMat)
{
	floorMat = _floorMat;
	wallMat = _wallMat;

	setupBoundingBox(width, height);
}

void Tile::drawWalls(const Material &wallMat) const
{
	if(fabsf(getTileHeight()) < FLT_EPSILON)
	{
		return;
	}

	wallMat.bind();

	// Draw the cube
	glBegin(GL_QUADS);

		// North
		glNormal3f(-1.0f, 0.0f, 0.0f);
		texCoord(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
		texCoord(1.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		texCoord(1.0f, 1.0f); glVertex3f(0.0f, 1.0f, 1.0f);
		texCoord(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);

		// South
		glNormal3f(1.0f, 0.0f, 0.0f);
		texCoord(0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		texCoord(0.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
		texCoord(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		texCoord(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);

		// East
		glNormal3f(0.0f, 1.0f, 0.0f);
		texCoord(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		texCoord(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
		texCoord(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		texCoord(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 1.0f);

		// West
		glNormal3f(0.0f, -1.0f, 0.0f);
		texCoord(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
		texCoord(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		texCoord(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
		texCoord(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();
}

void Tile::drawFloor(const Material &floorMat) const
{
	floorMat.bind();

	glBegin(GL_QUADS);
		glNormal3f( 0.0f, 1.0f, 0.0f);
		texCoord(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		texCoord(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 1.0f);
		texCoord(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		texCoord(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 1.0f);
	glEnd();
}

void Tile::setupBoundingBox(float width, float height)
{
	boundingBox.m_Max = vec3(width, width, height);
	boundingBox.m_Min = vec3(0, 0, 0);
	boundingBox.m_Pos = vec3(0,0,0); // we don't know where in the map we are
}

void Tile::draw(const vec2 &position) const
{
	const float width  = boundingBox.m_Max.x - boundingBox.m_Min.x;
	const float depth  = boundingBox.m_Max.y - boundingBox.m_Min.y;
	const float height = boundingBox.m_Max.z - boundingBox.m_Min.z;

	glPushMatrix();
	glTranslatef(position.x, position.y, -0.1f);
	glScalef(width, depth, height);

	drawFloor(floorMat);
	drawWalls(wallMat);

	glPopMatrix();
}
