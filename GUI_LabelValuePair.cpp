#include "GUI_Common.h"
#include "GUI_LabelValuePair.h"
#include "GUI_LabelValuePairState.h"

GUI_LabelValuePair::GUI_LabelValuePair(string _label, string _value, RECT _rect, int _id, GUI_ElementStyle *_style, GUI_font *_valuefont)
	: GUI_BaseElement(_rect, _id, _style), label(_label), valuefont(_valuefont)
{
	swapState(new GUI_LabelValuePairState(this));

	if (valuefont == NULL)
	{
		valuefont = style->GetFont();
	}
	createLabel();
	SetValue(_value);
}


GUI_LabelValuePair::~GUI_LabelValuePair()
{
}


void GUI_LabelValuePair::SetValue(string _value, bool hilighted)
{
	cState()->SetValue(_value);
	//erase the old value on the source surface
	RECT availablerect = _R(labelwidth, style->MarginTop(), width - style->MarginLeft(), height - style->MarginBottom());
	GUI_Draw::ColorFill(availablerect, src, style->BackgroundColor());
	//print the new text
	valuefont->Print(src, cState()->GetValue(), labelwidth, height / 2, availablerect, hilighted, T_LEFT, V_CENTER);
}


string GUI_LabelValuePair::GetValue()
{
	return cState()->GetValue();
}


void GUI_LabelValuePair::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{
	BLITDATA blitdata;
	calculateBlitData(xoffset + rect.left, yoffset + rect.top, drawablerect, blitdata);

	//width or height == 0 indicates that the element is completely outside its
	//parents rect, so no need to draw.
	if (blitdata.width > 0 && blitdata.height > 0)
	{
		oapiBlt(_tgt, src, &blitdata.tgtrect, &blitdata.srcrect, SURF_PREDEF_CK);
	}
}

void GUI_LabelValuePair::createLabel()
{
	labelwidth = font->GetTextWidth(string(label + " ")) + style->MarginLeft();
	src = GUI_Draw::createElementBackground(style, width, height);
	font->Print(src, label, style->MarginLeft(), height / 2, _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_LEFT, V_CENTER);
}


GUI_LabelValuePairState *GUI_LabelValuePair::cState()
{
	return (GUI_LabelValuePairState*)state;
}