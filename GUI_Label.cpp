#include "GUI_Common.h"
#include "GUI_Label.h"
#include "GUI_LabelState.h"

GUI_Label::GUI_Label(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style)
{
	swapState(new GUI_LabelState(this));
	cState()->SetText(_text);
	createResources();
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
		oapiBlt(_tgt, src, &blitdata.tgtrect, &blitdata.srcrect, SURF_PREDEF_CK);
	}
}

void GUI_Label::createResources()
{
	//destroy an existing surface before allocating it anew
	if (src != NULL)
	{
		oapiDestroySurface(src);
	}
	src = GUI_Draw::createElementBackground(style, width, height);
	font->Print(src, cState()->GetText(), width / 2, height / 2, _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_CENTER, V_CENTER);
}


void GUI_Label::ChangeText(string text)
{
	cState()->SetText(text);
	createResources();
}


GUI_LabelState *GUI_Label::cState()
{
	return (GUI_LabelState*)state;
}
