#include "GUI_Common.h"
#include "GUI_DynamicButton.h"
#include "math.h"

GUI_DynamicButton::GUI_DynamicButton(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style), text(_text)
{
	style = _style;
	font = style->GetFont();
	createButton();
}


GUI_DynamicButton::~GUI_DynamicButton()
{
}


void GUI_DynamicButton::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
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


bool GUI_DynamicButton::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (_event == LBUP)
	{
		return GUI_BaseElement::ProcessMe(_event, _x, _y);
	}
	return false;
}

//allocates the surface and draws the button on initialisation
void GUI_DynamicButton::createButton()
{
	//allocate own surface and fill with background color
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);

	Sketchpad *skp = oapiGetSketchpad(tgt);
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
	oapiReleaseSketchpad(skp);

	//print text
	font->Print(tgt, text, width / 2, height / 2, _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_CENTER, V_CENTER);

	//assign new surface as source
	src = tgt;
}

