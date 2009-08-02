#include "stdafx.h"
#include "random.h"

float angle_clamp(const float rads)
{
	const float P = float(2.0*M_PI);
	float clamped = rads;

	while(clamped < 0.0f)
	{
		clamped += P;
	}

	while(clamped > 2*P)
	{
		clamped -= P;
	}

	return clamped;
}

float getAngle( const vec2 &a, const vec2 &b )
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float rads = atan2f(dy, dx);
	float clamped = angle_clamp(rads);
	return clamped;
}

vec3 GetRandomVector(float length)
{
	return vec3(FRAND_RANGE(-1.0f,+1.0f),
		FRAND_RANGE(-1.0f,+1.0f),
		FRAND_RANGE(-1.0f,+1.0f)).getNormal() * length;
}

string toString(const any &value)
{
	string typeName = value.type().name();

	if(typeName == typeid(vec2).name())
	{
		return vec2::toString(any_cast<vec2>(value));
	}
	else if(typeName == typeid(vec3).name())
	{
		return vec3::toString(any_cast<vec3>(value));
	}
	else if(typeName == typeid(vec4).name())
	{
		return vec4::toString(any_cast<vec4>(value));
	}
	else if(typeName == typeid(mat4).name())
	{
		return mat4::toString(any_cast<mat4>(value));
	}
	else if(typeName == typeid(mat3).name())
	{
		return mat3::toString(any_cast<mat3>(value));
	}
	else if(typeName == typeid(float).name())
	{
		return ftos(any_cast<float>(value));
	}
	else if(typeName == typeid(int).name())
	{
		return itos(any_cast<int>(value));
	}
	else if(typeName == typeid(size_t).name())
	{
		return sizet_to_string(any_cast<size_t>(value));
	}
	else if(typeName == typeid(bool).name())
	{
		return any_cast<bool>(value) ? "true" : "false";
	}
	else
	{
		return "???";
	}
}

ILboolean ilLoadImage(const FileName &fileName)
{
	char *pszFileName = strdup(fileName.str());
	ILboolean r = ::ilLoadImage(pszFileName);
	delete[] pszFileName;
	return r;
}

unsigned int devil_loadImage(const FileName &fileName)
{
	unsigned int imageName = 0;
	ilGenImages(1, &imageName);
	ilBindImage(imageName);

	VERIFY(ilLoadImage(fileName), "Failed to load image: " + fileName.str());

	if(ilGetInteger(IL_ORIGIN_MODE) == IL_ORIGIN_LOWER_LEFT) // flip if needed
	{
		iluFlipImage();
	}

	return imageName;
}
