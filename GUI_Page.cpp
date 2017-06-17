#include "GUI_Common.h"
#include "GUI_Page.h"

GUI_Page::GUI_Page(RECT mRect, int _id, GUI_ElementStyle *_style, bool drawbackground)
: GUI_BaseElement(mRect, _id, _style)
{
	if (drawbackground)
	{
		createPage();
	}
}

GUI_Page::~GUI_Page(void)
{
}


void GUI_Page::initialiseState()
{
	//this element doesn't have a state.
}

bool GUI_Page::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
//a page returns always false when clicked. 
//Otherwise a click on an empty area of a page would always trigger a redraw, which is not desired.
{
	return false;
}


void GUI_Page::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{
	//check if this page was set to draw its background
	if (src != NULL)
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
}



void GUI_Page::AddChild(GUI_BaseElement *child)
{
	children.push_back(child);
	if (expandwithchildren)
	{
		reSize();
	}
}



void GUI_Page::reSize()
{
	int contentwidth = 0;
	int contentheight = 0;

	for (UINT i = 0; i < children.size(); ++i)
	{
		RECT childrect;
		children[i]->GetRect(childrect);

		if (childrect.right > contentwidth)
		{
			contentwidth = childrect.right;
		}
		if (childrect.bottom > contentheight)
		{
			contentheight = childrect.bottom;
		}
	}

	if (contentheight + style->MarginBottom() > rect.bottom)
	{
		rect.bottom = contentheight + style->MarginBottom();
	}
	if (contentwidth + style->MarginRight() > rect.right)
	{
		rect.right = contentwidth + style->MarginLeft() + style->MarginRight();
	}
}


void GUI_Page::createPage()
{
	//allocate own surface and fill with background color
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);

	Sketchpad *skp = oapiGetSketchpad(tgt);
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
	oapiReleaseSketchpad(skp);
	src = tgt;
}


