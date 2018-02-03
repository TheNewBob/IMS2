#include "GUI_Common.h"
#include "GUI_Page.h"
#include "GUI_ScrollBar.h"
#include "GUI_ScrollablePage.h"
#include "GUIentity.h"
#include "GUIplugin.h"
#include "GUImanager.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"

GUI_ScrollablePage::GUI_ScrollablePage(RECT mRect, int _id, GUI_ElementStyle *_style)
	: GUI_Page(mRect, _id, _style, false)
{
	createScrollbar();
	src = GUI_Looks::GetResource(this);
}


GUI_ScrollablePage::~GUI_ScrollablePage()
{
	delete scrollbar;
}


void GUI_ScrollablePage::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
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



void GUI_ScrollablePage::DrawChildren(SURFHANDLE _tgt, RECT &drawablerect, int xoffset, int yoffset)
{
	//calculating offset for children to draw
	int newxOffset = xoffset + rect.left;
	int newyOffset = yoffset + rect.top;

	//get the real drawable rect of this element
	RECT mydrawablerect;
	calculateMyDrawableRect(drawablerect, xoffset + rect.left, yoffset + rect.top, mydrawablerect);
	// Take the linestrength into account, otherwise elements might overlap the pages border when scrolling.
	mydrawablerect.top += style->LineWidth() + 1;   // Not sure why this additional pixel is necessary, but it is!
	mydrawablerect.bottom -= style->LineWidth();
	//factor the scroll position into the drawing position of the child elements
	int scrollyOffset = newyOffset - scrollbar->GetScrollPos();
	
	//draw children
	for (UINT i = 0; i < children.size(); ++i)
	{
		if (children[i]->GetFixed())
		{
			children[i]->Draw(_tgt, mydrawablerect, newxOffset, newyOffset);
		}
		else
		{
			children[i]->Draw(_tgt, mydrawablerect, newxOffset, scrollyOffset);
		}
	}

	//draw plugins
	for (UINT i = 0; i < plugins.size(); ++i)
	{
		if (plugins[i]->IsFixed())
		{
			plugins[i]->DrawChildren(_tgt, mydrawablerect, newxOffset, newyOffset);
		}
		else
		{
			plugins[i]->DrawChildren(_tgt, mydrawablerect, newxOffset, scrollyOffset);
		}
	}

}

void GUI_ScrollablePage::SetStyle(GUI_ElementStyle *style)
{
	if (style != this->style)
	{
		GUI_ElementStyle *scrollbarstyle = style->GetChildStyle() == NULL ? style : style->GetChildStyle();
		scrollbar->SetStyle(scrollbarstyle);
	}
	GUI_BaseElement::SetStyle(style);
}

void GUI_ScrollablePage::createScrollbar()
{
		//create the scrollbar
	int scrlbarwidth = GUI_Layout::EmToPx(1.5);
	GUI_ElementStyle *scrollbarstyle = style->GetChildStyle() == NULL ? style : style->GetChildStyle();

	if (scrollbar == NULL)
	{
		scrollbar = new GUI_ScrollBar(_R(width - scrlbarwidth, 0, width, height), GUI_SCROLLBAR, scrollbarstyle);
		scrollbar->SetNoBlit(true);
		scrollbar->SetScrollSpeed(10);
		scrollbar->SetFixed(true);
	}
	else 
	{
		scrollbar->SetStyle(scrollbarstyle);
	}

	//don't use the overloaded addchild method, because that one will to calculate
	//scrollrange for the contained elements. Since the scrollbar is the only element
	//that doesn't actually scroll with the page, it also should not afect the range.
	GUI_BaseElement::AddChild(scrollbar);

}


GUI_ElementResource *GUI_ScrollablePage::createResources()
{
	Olog::assertThat([this]() { return src == NULL; }, "GUI_ScrollablePage: Release old resource before creating it again!");
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);

	Sketchpad *skp = oapiGetSketchpad(tgt);
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
	oapiReleaseSketchpad(skp);

	//copy scrollbar to this surface
//	oapiBlt(tgt, scrollbar->GetSurface(), width - scrlbarwidth, 0, 0, 0, scrlbarwidth, height);
	int scrlbarwidth = scrollbar->GetWidth();
	RECT tgtrect = _R(width - scrlbarwidth, 0, width, height);
	RECT srcrect = _R(0, 0, scrlbarwidth, height);
	oapiBlt(tgt, scrollbar->GetSurface(), &tgtrect, &srcrect, SURF_PREDEF_CK);

	return new GUI_ElementResource(tgt);
}



void GUI_ScrollablePage::AddChild(GUI_BaseElement *child)
{
	GUI_BaseElement::AddChild(child);
	CalculateScrollRange();
}


void GUI_ScrollablePage::CalculateScrollRange()
{
	int contentheight = 0;
	for (UINT i = 0; i < children.size(); ++i)
	{
		RECT childrect;
		children[i]->GetRect(childrect);
		if (childrect.bottom > contentheight)
		{
			contentheight = childrect.bottom + 20;
		}
	}

	scrollbar->SetScrollRange(max(0, contentheight - rect.bottom));
}


int GUI_ScrollablePage::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//check the plugins first
	for (UINT i = plugins.size(); i > 0; --i)
	{
		//fixed elements do not scroll with the page
		int ypos = _y;
		if (!plugins[i - 1]->IsFixed())
		{
			ypos += scrollbar->GetScrollPos();
		}
		if (plugins[i - 1]->ProcessChildren(_event, _x, ypos))
		{
			//return own id if any plugins of this consumed the event to trigger redraw
			return id;
		}
	}


	for (UINT i = children.size(); i > 0; --i)
	{
		int eventId = -1;
		int ypos = _y;
		//fixed elements do not scroll with the page
		if (!children[i - 1]->GetFixed())
		{
			ypos += scrollbar->GetScrollPos();
		}
		eventId = children[i - 1]->ProcessMouse(_event, _x, ypos);

		if (eventId != -1)
		{
			return eventId;
		}
	}


	return -1;
}
