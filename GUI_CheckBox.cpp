#include "GUI_Common.h"
#include "GUI_CheckBox.h"

GUI_CheckBox::GUI_CheckBox(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style), text(_text), checked(true)
{
	createCheckBox();
	checked = true;
	ToggleChecked();
}


GUI_CheckBox::~GUI_CheckBox()
{
}

void GUI_CheckBox::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{
	BLITDATA blitdata;
	calculateBlitData(xoffset + rect.left, yoffset + rect.top, drawablerect, blitdata);
	oapiBlt(_tgt, src, blitdata.targetx, blitdata.targety, blitdata.srcx, blitdata.srcy, blitdata.width, blitdata.height);
}


bool GUI_CheckBox::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (_event == LBUP)
	{
		if (GUI_BaseElement::ProcessMe(_event, _x, _y))
		{
			ToggleChecked();
			return true;
		}
		return false;
	}
	return false;
}


bool GUI_CheckBox::ToggleChecked()
{
	checked = !checked;
	//copy the appropriate graphic in front of the text
	int fontheight = font->GetfHeight();
	if (checked)
	{
		oapiBlt(src, src, style->MarginLeft(), 0, width + fontheight, 0, fontheight, height);
	}
	else
	{
		oapiBlt(src, src, style->MarginLeft(), 0, width, 0, fontheight, height);
	}
	return checked;
}


void GUI_CheckBox::SetChecked(bool _checked)
{
	checked = _checked;
	//copy the appropriate graphic in front of the text
	int fontheight = font->GetfHeight();
	if (checked)
	{
		oapiBlt(src, src, style->MarginLeft(), 0, width + fontheight, 0, fontheight, height);
	}
	else
	{
		oapiBlt(src, src, style->MarginLeft(), 0, width, 0, fontheight, height);
	}
}

void GUI_CheckBox::createCheckBox()
{
	//the actual box of the checkbox will size itself to the font height
	int fontheight = font->GetfHeight();
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width + fontheight * 2, height);

	Sketchpad *skp = oapiGetSketchpad(tgt);
	//draw both the unchecked and the checked box after the width of the element
	GUI_Draw::DrawRectangle(_R(width, height / 2 - fontheight / 2, width + fontheight, height / 2 + fontheight / 2), skp, style);
	GUI_Draw::DrawRectangle(_R(width + fontheight, height / 2 - fontheight / 2, width + fontheight * 2, height / 2 + fontheight / 2), skp, style, true);

	oapiReleaseSketchpad(skp);

	//print text
	font->Print(tgt, text, fontheight + font->GetfWidth() + style->MarginLeft(), height / 2, _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_LEFT, V_CENTER);

	//assign new surface as source
	src = tgt;

}
