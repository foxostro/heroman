#ifndef _TEXT_H_
#define _TEXT_H_

#include "color.h"
#include "PropertyBag.h"

/**
Manages a single font and can print strings to the screen with this font
*/
class Font
{
public:
    /** Type for definition the dimensions of a character on-screen */
    enum FontSize
    {
        SizeHuge,
        SizeLarge,
        SizeNormal,
        SizeSmall
    };

	/** The origin of the font characters */
	vec3 origin;

private:
    /** Character in the font and measurements associated with it */
	class Character
	{
	public:
		Character()
		{
			charTex = 0;
			width = 1.0f;
			left = 0.0f;
		}

		/** The texture handle of the character */
		unsigned int charTex;

		/**
		Width of the character from the leftmost edge of the glyph to the
		rightmost edge (0.0 to 1.0, a portion of the Quad Width)
		*/
		float width;

		/**
		Distance from the edge of the quad to the leftmost edge of the glyph
		(0.0 to 1.0, a portion of the Quad Width)
		*/
		float left;
	};

	/** Characters in the font */
	Character characters[256];

	/** Height of a line (as a portion of the Quad Height) */
	float lineHeight;

	/** Spacing between characters */
	float spacing;

	/** Associates the TextWriter::FontSize tokens with an actual size measurement */
	map<FontSize, float> fontSize;

public:
	~Font();

	/** Constructor sets up the font */
	Font(const PropertyBag &data)
    {
	    setup(data);
    }

	/**
	Prints out a string assuming characters will face the camera head-on
	@param text The text to print
	@param color The coor of the text
	@param size The size of the text
	*/
	void write(const string &text, const color &color, FontSize size) const;

	/**
	Prints out a string using billboarded characters that will always face the
	camera head-on.
	@param text The text to print
	@param color The coor of the text
	@param size The size of the text
	@param cameraOrientation Camera orientation matrix to align the billboard
	@todo cameraOrientation should really be a mat3 and not a mat4
	*/
	void billboardWrite(const string &text,
	                    const color &color,
						FontSize size,
						mat3 cameraOrientation) const;

	/**
	Gets the dimensions of the given string
	@param text text
	@param size	The size of the text
	@return (width, height) of the text
	*/
	vec2 getDimensions(const string &text, FontSize size) const;

	/**
	Gets the height of a line of text in the given size
	@return The height of a line of text
	*/
	float getLineHeight(FontSize size) const;

private:
	/**
	Setup the font
	@param xml XML source for the font data
	*/
	void setup(const PropertyBag &xml);

	/** Loads font image and chops it up into individual characters */
	void loadFontImage(const FileName &fileName);

	/**
	Prints a single character to the screen
	@param a Corner of the character's quad
	@param b Corner of the character's quad
	@param c Corner of the character's quad
	@param d Corner of the character's quad
	@param z The character to print out
	@param color The color of the text
	@param size The size of the text
	*/
	void drawChar(const vec3 &a, const vec3 &b, const vec3 &c, const vec3 &d, char z, const color &color) const;

	/**
	Prints a single character to the screen and advances the cursor
	@param offset The position of the chracter on the screen.  Returns the position of the next character.
	@param character The character to print out
	@param color The coor of the text
	@param size The size of the text
	*/
	void putChar(vec3 *offset, char character, const color *color, FontSize size) const;

	/**
	Prints a single character to the screen and advances the cursor
	@param offset The position of the chracter on the screen.  Returns the position of the next character.
	@param character The character to print out
	@param color The coor of the text
	@param size The size of the text
	@param cameraOrientation Camera orientation matrix to align the billboard
	@todo cameraOrientation should really be a mat3 and not a mat4
	*/
	void putBillboardChar(vec3 start,
	                      vec3 *offset,
	                      char character,
	                      const color *color,
	                      FontSize size,
	                      mat3 cameraOrientation) const;

	/**
	Gets character information
	@param z Character
	@return Character information
	*/
	Character& getCharacter(char z);

	/**
	Gets character information
	@param z Character
	@return Character information
	*/
	const Character& getCharacter(char z) const;

	/**
	Gets character information
	(automatically used when compiling with unicode strings)
	@param z Character
	@return Character information
	*/
	Character& getCharacter(wchar_t z);

	/**
	Gets character information
	(automatically used when compiling with unicode strings)
	@param z Character
	@return Character information
	*/
	const Character& getCharacter(wchar_t z) const;
};

#endif
