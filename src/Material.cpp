#include "stdafx.h"
#include "Effect.h"
#include "TextureFactory.h"
#include "Material.h"

Material::Material()
{
	clear();
}

Material::Material(const Material &mat)
{
	copy(mat);
}

Material::Material(TextureFactory::Handle *texture)
{
	clear();
	setTexture(texture);
}

void Material::copy(const Material &mat)
{
    ambient = mat.ambient;
	diffuse = mat.diffuse;
	specular = mat.specular;
	shininess = mat.shininess;
	glow = mat.glow;
	texture = mat.texture;
}

void Material::clear()
{
	ambient = black;
	diffuse = color(0.8f,0.8f,0.8f,0.5f);
	specular = color(0.8f,0.8f,0.8f,1.0f);
	shininess = 16.0f;
	glow = false;
    texture = 0;
}

void Material::setTexture(TextureFactory::Handle *handle)
{
	ASSERT(handle!=0, "handle was null");
	texture = handle;
}

void Material::bind() const
{
	CHECK_GL_ERROR();

	if(texture != 0)
	{
		glBindTexture(GL_TEXTURE_2D, texture->getID());
		setTextureFilters();
		glEnable(GL_TEXTURE_2D);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
	glMaterialf(GL_FRONT,  GL_SHININESS, shininess);

	CHECK_GL_ERROR();
}

void Material::setTextureFilters()
{
	// trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}
