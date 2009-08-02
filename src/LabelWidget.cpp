#include "stdafx.h"
#include "Application.h"
#include "LabelWidget.h"

LabelWidget::LabelWidget(const string &label,
						 const vec2 &pos,
						 shared_ptr<Font> font)
: fontSize(Font::SizeNormal)
{
	setFont(font);
	setFontSize(Font::SizeNormal);
	setLabel(label);
	setColor(white);
	setRelativePos(pos);
}

LabelWidget::LabelWidget(const string &label,
						 const vec2 &pos,
						 const color &color,
						 Font::FontSize s,
						 shared_ptr<Font> font)
: fontSize(Font::SizeNormal)
{
	setFont(font);
	setFontSize(s);
	setLabel(label);
	setColor(color);
	setRelativePos(pos);
}

void LabelWidget::setLabel(const string& labelTxt)
{
	ASSERT(font != 0, "font was null!  Call setFont method first!");

	labelText = labelTxt;

	vec2 dim = font->getDimensions(labelText, fontSize);
	setWidth(dim.x);
	setHeight(dim.y);
}

void LabelWidget::setColor(const color &c)
{
	m_Color = c;
}

void LabelWidget::setFont(shared_ptr<Font> _font)
{
	font = _font;
}

void LabelWidget::setFontSize(Font::FontSize s)
{
	fontSize = s;
}

void LabelWidget::draw() const
{
	ASSERT(font, "font is null!  call setFont method first!");

	if(visible)
	{
		font->write(labelText, m_Color, fontSize);
	}
}
