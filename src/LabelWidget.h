#ifndef _LABELWIDGET_H_
#define _LABELWIDGET_H_

#include "tstring.h"
#include "color.h"
#include "Widget.h"
#include "Font.h"

class LabelWidget : public Widget
{
public:
	LabelWidget(const string &label,
				const vec2 &pos,
				shared_ptr<Font> font);

	LabelWidget(const string &label,
                const vec2 &pos,
                const color &color,
                Font::FontSize fontSize,
                shared_ptr<Font> font);

	/**
	Gets the label
	@return label
	*/
	inline const string& getLabel() const
	{
		return labelText;
	}

	/**
	Sets the label text
	@param label The new label text
	@todo Consider newline characters when calculating the bounds of the text
	*/
	void setLabel(const string& label);

	/**
	Sets the label font
	@param label The new label text
	*/
	void setFont(shared_ptr<Font> font);

	/**
	Sets the label color
	@param color The new label color
	*/
	void setColor(const color &color);

	/**
	Sets the label font size
	@param color The new label font size
	*/
    void setFontSize(Font::FontSize fontSize);

public:
	/** Label color */
	color m_Color;

protected:
	/** draws the widget*/
	virtual void draw() const;

private:
	/** Label text */
	string labelText;

	/** Font size */
    Font::FontSize fontSize;

	/** The current font */
	shared_ptr<Font> font;
};

#endif
