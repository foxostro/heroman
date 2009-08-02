#include "stdafx.h"
#include "ParticleElement.h"
#include "ParticleSystem.h"

ParticleElement::ParticleElement()
{
	resetMembers();
}

ParticleElement::ParticleElement(const PropertyBag &data,
								 Material *_material)
{
	load(data, _material);
}

ParticleElement::ParticleElement(const ParticleElement &element)
{
	position              = element.position;
	age                   = element.age;
	material              = element.material;
	lifeSpan              = element.lifeSpan;
	sizeMultiplier        = element.sizeMultiplier;
	graphSize             = element.graphSize;
	graphColor            = element.graphColor;
	initialPosition       = element.initialPosition;
	initialVelocity       = element.initialVelocity;
	initialRadialVelocity = element.initialRadialVelocity;
	graphAcceleration     = element.graphAcceleration;
}

void ParticleElement::load(const PropertyBag &data, Material *_material)
{
	resetMembers();

	ASSERT(_material, "Parameter was null: _material");
	material = _material;

	graphSize.load(data.getBag("size"));
	graphColor.load(data.getBag("color"));

	data.get("position", initialPosition);
	data.get("initialRadialVelocity", initialRadialVelocity);
	graphAcceleration.load(data.getBag("acceleration"));

	position = initialPosition;
}

void ParticleElement::draw(const mat4 &matrix) const
{
	vec3 A, B, C, D; // billboard vertices
	buildBillboardVertices(matrix, A, B, C, D);	
	drawBillboard(A, B, C, D);
}

void ParticleElement::update(float milliseconds)
{
	if(isDead())
		return;

	age += milliseconds;

	const float seconds = age / 1000.0f;

	position = (getAcceleration() * 0.5f * SQR(seconds)) +
	                    (initialVelocity * seconds) +
	                               initialPosition;
}

void ParticleElement::buildBillboardVertices(const mat4 & matrix,
											 vec3 &A,
											 vec3 &B,
											 vec3 &C,
											 vec3 &D) const
{
	float radius = sizeMultiplier*getSize();

	float x0 = matrix[0] - matrix[1];
	float x1 = matrix[0] + matrix[1];
	float y0 = matrix[4] - matrix[5];
	float y1 = matrix[4] + matrix[5];
	float z0 = matrix[8] - matrix[9];
	float z1 = matrix[8] + matrix[9];

	A = position + vec3(-x1, -y1, -z1)*radius;
	B = position + vec3( x0,  y0,  z0)*radius;
	C = position + vec3( x1,  y1,  z1)*radius;
	D = position + vec3(-x0, -y0, -z0)*radius;
}

void ParticleElement::drawBillboard(const vec3 &A,
									const vec3 &B,
									const vec3 &C,
									const vec3 &D) const
{
	glColor4fv(getColor());

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(A);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(B);
	glTexCoord2f(1.0f, 1.0f); glVertex3fv(C);
	glTexCoord2f(0.0f, 1.0f); glVertex3fv(D);
	glEnd();
}

void ParticleElement::resetMembers()
{
	material = 0;
	position = vec3(0,0,0);
	initialVelocity = vec3(0,0,0);
	initialRadialVelocity = 0.0f;
	initialPosition = vec3(0,0,0);
	lifeSpan = 1000.0f; 
	age = 0.0f;
}

void ParticleElement::setPosition(const vec3 &_position,
							      const vec3 &center,
							      float variance)
{
	const vec3 direction = vec3(_position - center).getNormal();
	const float addVariety = FRAND_RANGE(1-variance, 1+variance);
	initialVelocity = direction * (initialRadialVelocity*addVariety);
	initialPosition = _position;
	position = _position;
}

void ParticleElement::setPosition(const vec3 &_position)
{
	initialVelocity = vec3(0,0,0);
	initialPosition = _position;
	position = _position;
}

bool ParticleElement::operator<( const ParticleElement& o ) const
{
	if(isDead() && !o.isDead())
	{
		// dead particles get sorted to the end
		return false;
	}
	else if(!isDead() && o.isDead())
	{
		// dead particles get sorted to the end
		return true;
	}
	else
	{
		// sort by material
		return material < o.material;
	}
}