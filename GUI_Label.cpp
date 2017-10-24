#include "GUI_Common.h"
#include "GUI_Label.h"
#include "GUI_LabelState.h"

GUI_Label::GUI_Label(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style)
{
	swapState(new GUI_LabelState(this));
	cState()->SetText(_text);
	src = GUI_Looks::GetResource(this);
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

GUI_ElementResource *GUI_Label::createResources()
{
	assert(src == NULL && "Release old resource before creating it again!");
	
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);
	font->Print(tgt, cState()->GetText(), width / 2, height / 2, _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_CENTER, V_CENTER);

	return new GUI_ElementResource(tgt);
}

bool GUI_Label::IsResourceCompatibleWith(GUI_BaseElement *element)
{
	if (GUI_BaseElement::IsResourceCompatibleWith(element))
	{
		if (this == element || cState()->GetText() == ((GUI_Label*)element)->cState()->GetText()) return true;
	}
	return false;
}


void GUI_Label::ChangeText(string text)
{
	GUI_Looks::ReleaseResource(this);
	src = NULL;
	cState()->SetText(text);
	src = GUI_Looks::GetResource(this);
}


GUI_LabelState *GUI_Label::cState()
{
	return (GUI_LabelState*)state;
}
