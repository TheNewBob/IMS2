#include "GUI_Common.h"
#include "GUI_CheckBox.h"
#include "GUI_CheckBoxState.h"

GUI_CheckBox::GUI_CheckBox(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style), text(_text)
{
	swapState(new GUI_CheckBoxState(this));
	src = GUI_Looks::GetResource(this);
//	ToggleChecked();
}


GUI_CheckBox::~GUI_CheckBox()
{
}


void GUI_CheckBox::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{
	BLITDATA blitdata;
	calculateBlitData(xoffset + rect.left, yoffset + rect.top, drawablerect, blitdata);
	// Check how to compose the element depending on whether it is checked or not.
	int fontheight = font->GetfHeight();
	RECT boxsrcrect = boxsrcrect = _R(0, 0, fontheight, height);
	if (!cState()->GetChecked())
	{
		boxsrcrect.left += width;
		boxsrcrect.right += width;
	}
	RECT boxtgtrect = blitdata.tgtrect;
	boxtgtrect.right = boxtgtrect.left + fontheight;
	RECT textsrcrect = _R(fontheight, 0, width, height);
	RECT texttgtrect = blitdata.tgtrect;
	texttgtrect.left += fontheight;

	oapiBlt(_tgt, src, &texttgtrect, &textsrcrect, SURF_PREDEF_CK);
	oapiBlt(_tgt, src, &boxtgtrect, &boxsrcrect, SURF_PREDEF_CK);
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
	auto s = cState();
	s->SetChecked(!s->GetChecked());
	return s->GetChecked();
}



bool GUI_CheckBox::Checked()
{
	auto s = cState();
	return s->GetChecked();
}

void GUI_CheckBox::SetChecked(bool _checked)
{
	auto s = cState();
	s->SetChecked(_checked);
}

bool GUI_CheckBox::IsResourceCompatibleWith(GUI_BaseElement *element)
{
	if (GUI_BaseElement::IsResourceCompatibleWith(element))
	{
		if (text == ((GUI_CheckBox*)element)->text) return true;
	}
	return false;
}

GUI_ElementResource *GUI_CheckBox::createResources()
{
	Helpers::assertThat([this]() { return src == NULL; }, "Release old resource before creating it again!");
	//the actual box of the checkbox will size itself to the font height
	int fontheight = font->GetfHeight();
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width + fontheight * 2, height);

	Sketchpad *skp = oapiGetSketchpad(tgt);
	//draw the checked box in front of the text, the unchecked behind it. We'll blit selectively depending on state when drawing.
	GUI_Draw::DrawRectangle(_R(width, height / 2 - fontheight / 2, width + fontheight, height / 2 + fontheight / 2), skp, style);
	GUI_Draw::DrawRectangle(_R(0, height / 2 - fontheight / 2, fontheight, height / 2 + fontheight / 2), skp, style, true);

	oapiReleaseSketchpad(skp);

	//print text
	font->Print(tgt, text, fontheight + font->GetfWidth() + style->MarginLeft(), height / 2, 
		_R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), width - style->MarginBottom()),
		false, T_LEFT, V_CENTER);

	//assign new surface as source
	return new GUI_ElementResource(tgt);

}


GUI_CheckBoxState *GUI_CheckBox::cState()
{
	return (GUI_CheckBoxState*)state;
}
