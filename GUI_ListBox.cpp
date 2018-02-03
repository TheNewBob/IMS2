#include "GUI_common.h"
#include "GUI_ScrollBar.h"
#include "GUI_ListBox.h"
#include "GUI_Looks.h"
#include "GUI_ListBoxState.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"

GUI_ListBox::GUI_ListBox(RECT _mRect, int _id, GUI_ElementStyle *_style, bool _selectBox, bool _noSelect)
	: GUI_BaseElement(_mRect, _id, _style)
{
	swapState(new GUI_ListBoxState(this));
	//calculating how many lines the ListBox can display and max length of line
	style = _style;
	lineSpace = 2;
	nLines = (height - style->MarginBottom() - style->MarginTop()) / (font->GetfHeight() + lineSpace);
	//default justification is left
	listJustification = T_LEFT;
	//line position
	selected = 2;

	auto s = cState();
	s->SetSelectBox(_selectBox);
	s->SetnoSelect(_noSelect);
	createScrollbar();
	src = GUI_Looks::GetResource(this);
}

GUI_ListBox::~GUI_ListBox(void)
{
	cState()->ClearEntries();
	hilights.clear();
	selected = -1;
	delete scrollbar;
}


void GUI_ListBox::AddElement(string element, bool hilight)
//adds a new element to the list, and marks it either selected or not
{
	auto s = cState();
	s->AddEntry(element);
	if (s->GetSelectBox() && hilight) hilights.push_back(s->entries.size() - 1);
	scrollbar->SetScrollRange(s->entries.size() - nLines);
}



void GUI_ListBox::Draw(SURFHANDLE surf, RECT &drawablerect, int xoffset, int yoffset)
{

	BLITDATA blitdata;
	calculateBlitData(xoffset + rect.left, yoffset + rect.top, drawablerect, blitdata);

	//width or height == 0 indicates that the element is completely outside its
	//parents rect, so no need to draw.
	if (blitdata.width > 0 && blitdata.height > 0)
	{
		//draw background and frame
		oapiBlt(surf, src, &blitdata.tgtrect, &blitdata.srcrect, SURF_PREDEF_CK);
	}

	//find printable rect (
	RECT printableRect = _R(rect.left + xoffset + style->MarginLeft(), rect.top + yoffset + style->MarginTop(), rect.right + xoffset - scrlBarWidth - style->MarginLeft(), rect.bottom + yoffset - style->MarginBottom());

	//draw entries

	SetSelected(selected);
	int posInDrawList = 0;						//notes the position in the currently visible list as opposed to the entire list


	auto s = cState();

	for (UINT i = scrollbar->GetScrollPos(); i < s->entries.size() && int(i) < scrollbar->GetScrollPos() + nLines; ++i, ++posInDrawList)
	{
		int tgty = printableRect.top + (posInDrawList * (font->GetfHeight() + lineSpace));	
		
		if (!s->noSelect && !s->selectBox && selected == i || s->selectBox && IsHighlight(i)) 
		{
			font->Print(surf, s->entries[i], printableRect.left, tgty, printableRect, true, listJustification);
		}
		else
		{
			font->Print(surf, s->entries[i], printableRect.left, tgty, printableRect, false, listJustification);
		}
	}

}

bool GUI_ListBox::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	auto s = cState();
	if (_event == LBDOWN &&
		_x > rect.left && _x < rect.right - scrlBarWidth &&
		_y > rect.top && _y < rect.bottom)
	//click is in listbox area
	{
		int sel = (_y - (rect.top + style->MarginTop())) / (font->GetfHeight() + lineSpace) + scrollbar->GetScrollPos();
		if (sel >= 0 && sel < int(s->entries.size()))
		{
			selected = sel;			
			if (s->GetSelectBox())
			//if this listBox allows for multiselection
			{
				bool wasSelected = false;
				for (UINT i = 0; i < hilights.size(); ++i)
				{
					if (hilights[i] == sel)
					{	
						hilights.erase(hilights.begin() + i);
						wasSelected = true;
					}
				}
				if (!wasSelected)
					hilights.push_back(sel);
			}
			return true;
		}
	}
	return false;
}

int GUI_ListBox::GetSelected()
{
	if (selected >= int(cState()->entries.size()))
	{
		return -1;
	}
	return selected;
}

void GUI_ListBox::clear()
{
	cState()->ClearEntries();
	hilights.clear();
}

void GUI_ListBox::SetSelected(int _selected)
{
	if (_selected >= 0 && _selected < int(cState()->entries.size()))
	{
		selected = _selected;
	}
	else selected = -1;
}

std::string GUI_ListBox::GetSelectedText()
{
	//	sprintf(outVar, "%s", entries[selected].data());
	if (selected != -1)
	{
		return cState()->entries[selected];
	}
	else
	{
		return "";
	}
}

int GUI_ListBox::NumEntries()
{
	return cState()->entries.size();
}

bool GUI_ListBox::IsHighlight(UINT index)
{
	auto s = cState();
	if (!s->GetSelectBox() || index > s->entries.size()) return false;

	for (UINT i = 0; i < hilights.size(); ++i)
	{
		if (hilights[i] == index) return true;
	}

	return false;
}


void GUI_ListBox::SetStyle(GUI_ElementStyle *style)
{
	if (style != this->style)
	{
		GUI_ElementStyle *scrollbarstyle = style->GetChildStyle() == NULL ? style : style->GetChildStyle();
		scrollbar->SetStyle(scrollbarstyle);
	}
	GUI_BaseElement::SetStyle(style);
}

void GUI_ListBox::createScrollbar()
{
	//create the scrollbar
	scrlBarWidth = GUI_Layout::EmToPx(1.5);
	GUI_ElementStyle *scrollbarstyle = style->GetChildStyle() == NULL ? style : style->GetChildStyle();

	if (scrollbar == NULL)
	{
		scrollbar = new GUI_ScrollBar(_R(width - scrlBarWidth, 0, width, height), GUI_SCROLLBAR, scrollbarstyle);
		AddChild(scrollbar);
	}
	else
	{
		scrollbar->SetStyle(scrollbarstyle);
	}
}

GUI_ElementResource *GUI_ListBox::createResources()
{
	Olog::assertThat([this]() { return src == NULL; }, "GUI_ListBox: Release old resource before creating it again!");

	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);
	Sketchpad *skp = oapiGetSketchpad(tgt);
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
	oapiReleaseSketchpad(skp);

	//copy scrollbar to this surface
	//oapiBlt(tgt, scrollbar->GetSurface(), width - scrlBarWidth, 0, 0, 0, scrlBarWidth, height);
	RECT tgtrect = _R(width - scrlBarWidth, 0, width, height);
	RECT srcrect = _R(0, 0, scrlBarWidth, height);
	SURFHANDLE test = scrollbar->GetSurface();
	oapiBlt(tgt, test, &tgtrect, &srcrect, SURF_PREDEF_CK);

	return new GUI_ElementResource(tgt);
}


int GUI_ListBox::GetNeededListBoxHeight(int numlines, string styleid)
{
	GUI_ElementStyle *style = GUI_Looks::GetStyle(styleid);
	int fontheight = style->GetFont()->GetfHeight();
	//2 pixels spacing between lines
	return numlines * (fontheight + 2) + style->MarginTop() + style->MarginBottom();
}


GUI_ListBoxState *GUI_ListBox::cState()
{
	return (GUI_ListBoxState*)state;
}