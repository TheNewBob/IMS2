#include "GUI_Common.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"
#include "GUI_Page.h"

GUI_Page::GUI_Page(RECT mRect, int _id, GUI_ElementStyle *_style, bool drawbackground)
	: GUI_BaseElement(mRect, _id, _style), drawBackground(drawbackground)
{
	if (drawbackground)
	{
		src = GUI_Looks::GetResource(this);
	}
}

GUI_Page::~GUI_Page(void)
{
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
			oapiBlt(_tgt, src, &blitdata.tgtrect, &blitdata.srcrect, SURF_PREDEF_CK);
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


GUI_ElementResource *GUI_Page::createResources()
{
	assert(src == NULL && "Release old resource before creating it again!");

	if (drawBackground)
	{
		//allocate own surface and fill with background color
		SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);

		Sketchpad *skp = oapiGetSketchpad(tgt);
		GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
		oapiReleaseSketchpad(skp);
		return new GUI_ElementResource(tgt);
	}
	return NULL;
}

LAYOUTDATA GUI_Page::getLayoutDataForElement(string elementid, LAYOUTCOLLECTION *layouts)
{
	vector<string> ignore_none;
	return getLayoutDataForElement(elementid, layouts, ignore_none);
}


LAYOUTDATA GUI_Page::getLayoutDataForElement(string elementid, LAYOUTCOLLECTION *layouts, vector<string> &ignore_fields)
{
	//check how much space we have to draw on and which layout we have to use for that.
	GUI_Layout *usedlayout = layouts->GetLayoutForWidth(width);

	//Get the elements rect, then translate its position by the left and top margins
	return usedlayout->GetLayoutDataForField(elementid, width, ignore_fields);
}


string GUI_Page::getElementStyle(string elementid, LAYOUTCOLLECTION *layouts)
{
	GUI_Layout *usedlayout = layouts->GetLayoutForWidth(width);
	return usedlayout->GetStyleForField(elementid);
}