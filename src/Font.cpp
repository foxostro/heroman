#include "stdafx.h"
#include "Font.h"
#include <SDL/SDL_image.h>

Font::~Font()
{
	for(int i=0; i<256; ++i)
	{
		glDeleteTextures(1, &characters[i].charTex);
		characters[i].charTex = 0;
	}
}

void Font::setup(const PropertyBag &data)
{
	fontSize.clear();
	fontSize[Font::SizeHuge]   = data.getFloat("huge");
	fontSize[Font::SizeLarge]  = data.getFloat("large");
	fontSize[Font::SizeNormal] = data.getFloat("normal");
	fontSize[Font::SizeSmall]  = data.getFloat("small");

	spacing = data.getFloat("spacing");
	lineHeight = data.getFloat("lineHeight");

	loadFontImage(data.getFileName("image"));
}

Font::Character& Font::getCharacter(char z)
{
	ASSERT(z >= 0, "Invalid character");

	return characters[  (size_t)((z >= 0) ? z : '?')  ];
}

const Font::Character& Font::getCharacter(char z) const
{
	return characters[  (size_t)((z >= 0) ? z : '?')  ];
}

Font::Character& Font::getCharacter(wchar_t z)
{
	ASSERT(z < 128, "Invalid character");

	return characters[  (size_t)((z < 128) ? z : '?')  ];
}

const Font::Character& Font::getCharacter(wchar_t z) const
{
	ASSERT(z < 128, "Invalid character");

	return characters[  (size_t)((z < 128) ? z : '?')  ];
}

void Font::drawChar(const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &d, char z, const color &color) const
{
	glBindTexture(GL_TEXTURE_2D, getCharacter(z).charTex);
	glColor4fv(color);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3fv(a);
		glTexCoord2f(1.0f, 0.0f); glVertex3fv(b);
		glTexCoord2f(1.0f, 1.0f); glVertex3fv(c);
		glTexCoord2f(0.0f, 1.0f); glVertex3fv(d);
	glEnd();
}

void Font::putChar(vec3 *offset, char character, const color *color, FontSize size) const
{
	const float _size = fontSize.find(size)->second;

	float w = 0.0f;
	float h = 0.0f;
	vec3 right;
	vec3 up;
	vec3 nextOffset;

	w = _size / 100.0f;
	h = _size / 100.0f;

	w = _size;
	h = _size;
	right = vec3(1, 0, 0);
	up = vec3(0, 1, 0);

	if(character == '\n')
	{
		nextOffset.x = 0;
		nextOffset.y = (*offset).y - h*lineHeight;
	}
	else
	{
		/*               center the quad          place it        shift left by an offset  */
		const vec3 a = -right*(w*0.5f)        + (*offset) - right*w*getCharacter(character).left;
		const vec3 b =  right*(w*0.5f)        + (*offset) - right*w*getCharacter(character).left;
		const vec3 c =  right*(w*0.5f) + up*h + (*offset) - right*w*getCharacter(character).left;
		const vec3 d = -right*(w*0.5f) + up*h + (*offset) - right*w*getCharacter(character).left;

		drawChar(a, b, c, d, character, *color);

		float offsetRight = w * (getCharacter(character).width + spacing);

		nextOffset = (*offset) + right*offsetRight;
	}

	(*offset) = nextOffset;
}

void Font::putBillboardChar(vec3 start,
							vec3 *offset,
							char character,
							const color *color,
							FontSize size,
							mat3 cameraOrientation) const
{
	const float _size = fontSize.find(size)->second;

	float w = _size / 100.0f;
	float h = _size / 100.0f;

	const vec3 right = cameraOrientation.getAxisX();
	const vec3 up = cameraOrientation.getAxisY();

	vec3 nextOffset;

	if(character == '\n')
	{
		nextOffset = (*offset) + -up*(h*lineHeight);
	}
	else
	{
		/*               center the quad          place it        shift left by an offset  */
		const vec3 a = -right*(w*0.5f)        + (*offset) - right*w*getCharacter(character).left;
		const vec3 b =  right*(w*0.5f)        + (*offset) - right*w*getCharacter(character).left;
		const vec3 c =  right*(w*0.5f) + up*h + (*offset) - right*w*getCharacter(character).left;
		const vec3 d = -right*(w*0.5f) + up*h + (*offset) - right*w*getCharacter(character).left;

		drawChar(a, b, c, d, character, *color);

		float offsetRight = w * (getCharacter(character).width + spacing);

		nextOffset = (*offset) + right*offsetRight;
	}

	(*offset) = nextOffset;
}

void Font::billboardWrite(const string &text,
						  const color &color,
						  FontSize size,
						  mat3 cameraOrientation) const
{
	CHECK_GL_ERROR();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	// Enable blending, to smooth the characters' edges and show through to the background
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);

	// Write out the string
	vec3 offset;
	vec3 start = offset;
	for_each(text.begin(), text.end(), bind(&Font::putBillboardChar, this, start, &offset, _1, &color, size, cameraOrientation));

	glPopAttrib();
	CHECK_GL_ERROR();
}

void Font::write(const string &text, const color &color, FontSize size) const
{
	CHECK_GL_ERROR();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1f);

		// Enable blending, to smooth the characters' edges and show through to the background
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_BLEND);

		// Write out the string
		vec3 offset;
		for_each(text.begin(), text.end(), bind(&Font::putChar,
		                                        this,
												&offset,
												_1,
												&color,
												size));

	glPopAttrib();
	CHECK_GL_ERROR();
}

vec2 Font::getDimensions(const string &text, FontSize size) const
{
	float row_width=0;
	float max_width=0;
	int numLines=1;

	for(string::const_iterator i = text.begin(); i != text.end(); ++i)
	{
		if((*i) == '\n')
		{
			numLines++;
			max_width = max(max_width, row_width);
			row_width = 0;
		}
		else
		{
			row_width += getCharacter(*i).width;
		}
	}

	max_width = max(max_width, row_width);

	const float _size = fontSize.find(size)->second;

	return vec2(max_width, numLines*lineHeight) * _size;
}

float Font::getLineHeight(FontSize size) const
{
    return lineHeight * fontSize.find(size)->second;
}

void Font::loadFontImage(const FileName &fileName)
{
	CHECK_GL_ERROR();
	
	unsigned int imageName = devil_loadImage(fileName);

	// Get image data
	unsigned char *font	= ilGetData();
	int fontWidth		= ilGetInteger(IL_IMAGE_WIDTH);
	int fontHeight		= ilGetInteger(IL_IMAGE_HEIGHT);
	int bytesPerPixel	= ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	bool alpha          = bytesPerPixel==4;

	ASSERT(font!=0,       "font was NULL");
	ASSERT(fontWidth>16,  "font width is too narrow");
	ASSERT(fontHeight>16, "font height is too narrow");

	// The number of columns and rows of characters contained in the font
	const int numCols = 16;
	const int numRows = 16;

	// Each character in the font has these dimensions
	const GLint width = fontWidth / 16;
	const GLint height = fontHeight / 16;

	// Create a buffer
	const size_t size = width * height * bytesPerPixel;
	unsigned char *buffer = new unsigned char[size];
	memset(buffer, 0, size);

	// Create sub textures for each character
	glEnable(GL_TEXTURE_2D);
	for(unsigned char c=0; c<255; ++c)
	{
		int charX = c % 16;
		int charY = 15 - (c - charX) / 16;
		int x = charX * fontWidth / numCols;
		int y = charY * fontHeight / numRows;

		int imgLeft=width;
		int imgRight=0;

		// Copy the character into the smaller buffer
		for(int row=0; row<height; ++row)
		{
			size_t font_row = row + y;
			size_t font_rowLength = fontWidth * bytesPerPixel;
			size_t x_offset = x * bytesPerPixel;
			size_t font_idx = font_row * font_rowLength + x_offset;

			size_t char_row = row;
			size_t char_rowLength = width * bytesPerPixel;
			size_t char_idx = char_row * char_rowLength;

			memcpy(buffer + char_idx, font + font_idx, char_rowLength);

			const int threshold = 96;

			int left=0;
			while(left < width && buffer[char_idx + left*bytesPerPixel + 3] < threshold) ++left;

			int right=width-1;
			while(right >= 0 && buffer[char_idx + right*bytesPerPixel + 3] < threshold) --right;

			imgLeft = min(imgLeft, left);
			imgRight = max(imgRight, right);
		}

		if(imgLeft >= imgRight)
		{
			characters[c].width = 0.4f;
			characters[c].left = 0.0f;
		}
		else
		{
			characters[c].width = (imgRight - imgLeft) / (float)width;
			characters[c].left = imgLeft / (float)width;
		}

		// Create a new texture object
		characters[c].charTex=0;
		glGenTextures(1, &characters[c].charTex);

		// and bind it as the present texture
		glBindTexture(GL_TEXTURE_2D, characters[c].charTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Now build mipmaps from the texture data
		gluBuild2DMipmaps(GL_TEXTURE_2D,
		                  bytesPerPixel,
		                  width,
		                  height,
		                  alpha ? GL_RGBA : GL_RGB,
		                  GL_UNSIGNED_BYTE,
		                  buffer);
	} // loop

	// Delete the buffer
	delete[] buffer;
	buffer=0;

	// Allow DevIL to free image data on its end
	ilDeleteImage(imageName);

	TRACE(string("Created font: ") + fileName.str());

	CHECK_GL_ERROR();
}
