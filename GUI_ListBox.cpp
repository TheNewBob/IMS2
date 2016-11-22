#include "GUI_common.h"
#include "GUI_ScrollBar.h"
#include "GUI_ListBox.h"
#include "FontsAndStyles.h"


GUI_ListBox::GUI_ListBox(RECT _mRect, int _id, GUI_ElementStyle *_style, GUI_ElementStyle *_scrollbarstyle, bool _selectBox, bool _noSelect)
	: GUI_BaseElement(_mRect, _id, _style)
{
	//calculating how many lines the ListBox can display and max length of line
	style = _style;
	lineSpace = 2;
	nLines = (height - style->MarginBottom() - style->MarginTop()) / (font->GetfHeight() + lineSpace);
	//default justification is left
	listJustification = T_LEFT;
	//line position
	selected = 2;

	selectBox = _selectBox;
	noSelect = _noSelect;
	createListBox(_scrollbarstyle);
}

GUI_ListBox::~GUI_ListBox(void)
{
	entries.clear();
	hilights.clear();
	selected = -1;
	delete scrollbar;
}


void GUI_ListBox::AddElement(string element, bool hilight)
//adds a new element to the list, and marks it either selected or not
{
	entries.push_back(element);
	if (selectBox && hilight) hilights.push_back(entries.size() - 1);
	scrollbar->SetScrollRange(entries.size() - nLines);
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
		oapiBlt(surf, src, blitdata.targetx, blitdata.targety, blitdata.srcx, blitdata.srcy, blitdata.width, blitdata.height);
	}

	//find printable rect (
	RECT printableRect = _R(rect.left + xoffset + style->MarginLeft(), rect.top + yoffset + style->MarginTop(), rect.right + xoffset - scrlBarWidth - style->MarginLeft(), rect.bottom + yoffset - style->MarginBottom());

	//draw entries

	SetSelected(selected);
	int posInDrawList = 0;						//notes the position in the currently visible list as opposed to the entire list




	for (UINT i = scrollbar->GetScrollPos(); i < entries.size() && int(i) < scrollbar->GetScrollPos() + nLines; ++i, ++posInDrawList)
	{
		int tgty = printableRect.top + (posInDrawList * (font->GetfHeight() + lineSpace));	
		
		if (!noSelect && !selectBox && selected == i || selectBox && IsHighlight(i)) 
		{
			font->Print(surf, entries[i], printableRect.left, tgty, printableRect, true, listJustification);
		}
		else
		{
			font->Print(surf, entries[i], printableRect.left, tgty, printableRect, false, listJustification);
		}
	}

}

bool GUI_ListBox::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	
	if (_event == LBDOWN &&
		_x > rect.left && _x < rect.right - scrlBarWidth &&
		_y > rect.top && _y < rect.bottom)
	//click is in listbox area
	{
		int sel = (_y - (rect.top + style->MarginTop())) / (font->GetfHeight() + lineSpace) + scrollbar->GetScrollPos();
		if (sel >= 0 && sel < int(entries.size()))
		{
			selected = sel;			
			if (selectBox)
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
	if (selected >= int(entries.size()))
	{
		return -1;
	}
	return selected;
}

void GUI_ListBox::clear()
{
	entries.clear();
	hilights.clear();
}

void GUI_ListBox::SetSelected(int _selected)
{
	if (_selected >= 0 && _selected < int(entries.size()))
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
		return entries[selected];
	}
	else
	{
		return "";
	}
}

int GUI_ListBox::NumEntries()
{
	return entries.size();
}

bool GUI_ListBox::IsHighlight(UINT index)
{
	if (!selectBox || index > entries.size()) return false;

	for (UINT i = 0; i < hilights.size(); ++i)
	{
		if (hilights[i] == index) return true;
	}

	return false;
}

void GUI_ListBox::createListBox(GUI_ElementStyle *scrollbarstyle)
{
	if (scrollbarstyle == NULL)
	{
		scrollbarstyle = style;
	}
	//create the scrollbar
	scrlBarWidth = 24;
	scrollbar = new GUI_ScrollBar(_R(width - scrlBarWidth, 0, width, height), GUI_SCROLLBAR, scrollbarstyle);
	AddChild(scrollbar);

	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);
	//copy scrollbar to this surface
	oapiBlt(tgt, scrollbar->GetSurface(), width - scrlBarWidth, 0, 0, 0, scrlBarWidth, height);
	Sketchpad *skp = oapiGetSketchpad(tgt);
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
	oapiReleaseSketchpad(skp);
	src = tgt;
}


int GUI_ListBox::GetNeededListBoxHeight(int numlines, GUI_STYLE styleid)
{
	GUI_ElementStyle *style = FontsAndStyles::GetInstance()->GetStyle(styleid);
	int fontheight = style->GetFont()->GetfHeight();
	//2 pixels spacing between lines
	return numlines * (fontheight + 2) + style->MarginTop() + style->MarginBottom();
}

