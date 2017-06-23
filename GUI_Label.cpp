#include "GUI_Common.h"
#include "GUI_Label.h"


GUI_Label::GUI_Label(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style), text(_text)
{
	createLabel();
}


GUI_Label::~GUI_Label()
{
}


void GUI_Label::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{
	BLITDATA blitdata;
	calculateBlitData(xoffset + rect.left, yoffset + rect.top, drawablerect, blitdata);

	//width or height == 0 indicates that the element is completely outside its
	//parents rect, so no need to draw.
	if (blitdata.width > 0 && blitdata.height > 0)
	{
		oapiBlt(_tgt, src, blitdata.targetx, blitdata.targety, blitdata.srcx, blitdata.srcy, blitdata.width, blitdata.height);
	}
}

void GUI_Label::createLabel()
{
	//destroy an existing surface before allocating it anew
	if (src != NULL)
	{
		oapiDestroySurface(src);
	}
	src = GUI_Draw::createElementBackground(style, width, height);
	font->Print(src, text, width / 2, height / 2, _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_CENTER, V_CENTER);
}


void GUI_Label::ChangeText(string text)
{
	this->text = text;
	createLabel();
}
