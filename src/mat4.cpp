#include "stdafx.h"
#include "mat4.h"

vec4 mat4::transformVector(const vec4 &v) const
{
	// the return vec4
	vec4 temp;

	// temp = v * mt;
	temp.x = v.x*m[0] + v.y*m[4] + v.z*m[8] + m[12];
	temp.y = v.x*m[1] + v.y*m[5] + v.z*m[9] + m[13];
	temp.z = v.x*m[2] + v.y*m[6] + v.z*m[10]+ m[14];
	temp.w = v.x*m[3] + v.y*m[7] + v.z*m[11]+ m[15];
	
	//temp = temp + getPos();

	// and return it
	return temp;
}

vec3 mat4::transformVector(const vec3 &v) const
{
	// the return vec3
	vec3 temp;

	// temp = v * mt;
	temp.x = v.x*m[0] + v.y*m[4] + v.z*m[8] + m[12];
	temp.y = v.x*m[1] + v.y*m[5] + v.z*m[9] + m[13];
	temp.z = v.x*m[2] + v.y*m[6] + v.z*m[10]+ m[14];

	//temp = temp + getPos();

	// and return it
	return temp;
}

void mat4::rotate(float radians, const vec3 &axis)
{
	identity();

	float degrees = radians * 180.0f / (float)M_PI;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(degrees, axis.x, axis.y, axis.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glPopMatrix();
}

void mat4::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up)
{
	vec3 z = up;
	vec3 y = vec3(eye-center).getNormal();
	vec3 x = z.cross(y);

	identity();
	setAxisX(x);
	setAxisY(y);
	setAxisZ(z);
	setPos(eye);
}
