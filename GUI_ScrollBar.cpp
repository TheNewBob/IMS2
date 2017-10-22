#include "GUI_Common.h"
#include "GUI_ScrollBar.h"


GUI_ScrollBar::GUI_ScrollBar(RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style)
{
	src = GUI_Looks::GetResource(this);
//	src = new GUI_ElementStyle("test");
}


GUI_ScrollBar::~GUI_ScrollBar()
{
}


void GUI_ScrollBar::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
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


GUI_ElementResource *GUI_ScrollBar::createResources()
{
	assert(src == NULL && "Release old resource before creating it again!");

	//allocate own surface and fill with background color
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);

	Sketchpad *skp = oapiGetSketchpad(tgt);
	//draw the top end of the scrollbar
	RECT temprect = _R(0, 0, width, width);
	//we face the problem that we need to draw a rectangle, but it's only the upper part of the actual scrollbar. 
	//Hence bottom corners must not be rounded even if the style says so. We need to extract the top corners.
	GUI_ElementStyle *tempstyle = new GUI_ElementStyle("temp", style);
	DWORD tempcorners = style->GetCorners();
	string corners = "";
	if ((UPPER_RIGHT & tempcorners) == UPPER_RIGHT)
	{
		corners = "1,";
	}
	if ((UPPER_LEFT & tempcorners) == UPPER_LEFT)
	{
		corners += "2";
	}
	tempstyle->Set(roundcorners, corners);
	GUI_Draw::DrawRectangle(temprect, skp, tempstyle, true);
	
	//now repeat that for the lower end of the scrollbar
	temprect = _R(0, height - width, width, height);
	tempcorners = style->GetCorners();
	corners = "";
	if ((LOWER_LEFT & tempcorners) == LOWER_LEFT)
	{
		corners = "3,";
	}
	if ((LOWER_RIGHT & tempcorners) == LOWER_RIGHT)
	{
		corners += "4";
	}
	tempstyle->Set(roundcorners, corners);
	GUI_Draw::DrawRectangle(temprect, skp, tempstyle, true);

	//next we have to draw the arrows, for which we need a vertex list. from lower right, counter-clockwise:
	IVECTOR2 tri[3];
	int arbitrarymeasure = width / 6;			//it has to look good, not be mathematically precise. I'm pragmatic that way...
	tri[0].x = width - arbitrarymeasure;
	tri[0].y = width - arbitrarymeasure;
	tri[1].x = width / 2;
	tri[1].y = arbitrarymeasure;
	tri[2].x = arbitrarymeasure;
	tri[2].y = width - arbitrarymeasure;
	tempstyle->SetFillColor(style->BackgroundColor());
	GUI_Draw::DrawPolygon(tri, 3, skp, tempstyle);
	//same for the bottom arrow, from upper-right:
	tri[0].x = width - arbitrarymeasure;
	tri[0].y = height - (width - arbitrarymeasure);
	tri[1].x = arbitrarymeasure;
	tri[1].y = height - (width - arbitrarymeasure);
	tri[2].x = width / 2;
	tri[2].y = height - arbitrarymeasure;
	GUI_Draw::DrawPolygon(tri, 3, skp, tempstyle);

	//finally, draw the lines holding the top and bottom together
	int lineinset = (int)ceil((float)style->LineWidth() / 2.0);		//compensates for linewidth so we don't draw out of the rectangle
	GUI_Draw::DrawLine(lineinset, width, lineinset, height - width + 1, skp, style);
	GUI_Draw::DrawLine(width - lineinset, width, width - lineinset, height - width + 1, skp, style);

	oapiReleaseSketchpad(skp);

	return new GUI_ElementResource(tgt);
}


bool GUI_ScrollBar::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (_event == LBDOWN || _event == LBPRESSED)
	{
		if (_x > rect.left && _x < rect.right &&
			_y > rect.top && _y < rect.bottom)
		{
			//check if button was pressed
			if (_y < rect.top + width)
			{
				//scroll up
				scrollpos = max(0, scrollpos - scrollspeed);
				return true;
			}
			else if (_y > rect.bottom - width)
			{
				//scroll down
				scrollpos = min(scrollrange, scrollpos + scrollspeed);
				return true;
			}
		}
	}
	return false;
}