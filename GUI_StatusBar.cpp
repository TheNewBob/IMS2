#include "GUI_Common.h"
#include "GUI_StatusBar.h"
#include "GUI_StatusBarState.h"

GUI_StatusBar::GUI_StatusBar(RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_BaseElement(_rect, _id, _style)
{
	swapState(new GUI_StatusBarState(this));
	src = GUI_Looks::GetResource(this);
}


GUI_StatusBar::~GUI_StatusBar()
{
	if (statussrf != NULL)
	{
		oapiDestroySurface(statussrf);
	}
}


void GUI_StatusBar::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{
	if (updatenextframe)
	{
		//if the bar does a self-triggered redraw, it means its fill status has changed
		updatenextframe = false;
		//prepare the new text to be printed in the status bar
		string statustext;
		createStatusString(statustext);
		//then update the bitmaps
		prepareFullStatusString(statustext);
		prepareEmptyStatusString(statustext);
	}


	BLITDATA blitdata;
	calculateBlitData(xoffset + rect.left, yoffset + rect.top, drawablerect, blitdata);


	//we'll need the linewidth and corner radius to properly position the text with no issues
	int cr = style->CornerRadius();
	int lw = style->LineWidth();

	auto s = cState();
	//width or height == 0 indicates that the element is completely outside its
	//parents rect, so no need to draw.
	if (blitdata.width > 0 && blitdata.height > 0)
	{
		if (s->GetFillStatus() == 1.0)
		{
			//the status bar is completely filled, just blit the whole filled bar
			blitdata.srcrect.top = 0;
			blitdata.srcrect.bottom = height;
			oapiBlt(_tgt, statussrf, &blitdata.tgtrect, &blitdata.srcrect, SURF_PREDEF_CK);
		}
		else if (s->GetFillStatus() == 0.0)
		{
			//the status bar is completely empty, just blit the whole empty bar
			blitdata.srcrect.top = height;
			blitdata.srcrect.bottom = height * 2;
			oapiBlt(_tgt, statussrf, &blitdata.tgtrect, &blitdata.srcrect, SURF_PREDEF_CK);
		}
		else
		{
			//the status bar is partly full, partly empty, we need two operations
			//calculate the split between full and empty status bar
			int splitfromleft = (int)(width * s->GetFillStatus());
			int splitfromright = width - splitfromleft;

			//calculate the source and target rects for the full part of the status bar and blit it.
			RECT fullsrcrect = _R(blitdata.srcrect.left,
				0,
				blitdata.srcrect.right - splitfromright,
				height);

			RECT fulltgtrect = blitdata.tgtrect;
			fulltgtrect.right -= splitfromright;
			
			oapiBlt(_tgt, statussrf, &fulltgtrect, &fullsrcrect, SURF_PREDEF_CK);

			//calculate the source and target rects for the empty part of the status bar and blit it.
			RECT emptysrcrect = _R(blitdata.srcrect.left + splitfromleft,
				height,
				blitdata.srcrect.right,
				height * 2);

			RECT emptytgtrect = blitdata.tgtrect;
			emptytgtrect.left += splitfromleft;

			oapiBlt(_tgt, statussrf, &emptytgtrect, &emptysrcrect, SURF_PREDEF_CK);
		}
	}
}

bool GUI_StatusBar::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	return false;
}

GUI_ElementResource *GUI_StatusBar::createResources()
{
	assert(src == NULL && "Release old resource before creating it again!");
	assert(height > font->GetfHeight() && "StatusBar must be taller than its font!");

/*	if (statussrf != NULL)
	{
		oapiDestroySurface(statussrf);
	}*/
	//allocate own surface and fill with background color
	//two complete representations of the bar are needed: one filled, one empty.
	//And we need asurface on which to draw the text being shown in the bar
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height * 2);
	Sketchpad *skp = oapiGetSketchpad(tgt);

	//draw the filled image of the status bar
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style, true);
	//draw the empty image of the status bar
	GUI_Draw::DrawRectangle(_R(0, height, width, height * 2), skp, style);

	oapiReleaseSketchpad(skp);

	//assign new surface as source
	return new GUI_ElementResource(tgt);
}


bool GUI_StatusBar::updateMe()
{
	if (updatenextframe)
	{
		bool bugme = true;
	}
	return updatenextframe;
}

void GUI_StatusBar::SetFillStatusByFraction(double fraction)
{
	assert(fraction >= 0.0 && fraction <= 1.0 && "Passed an invalid fraction!");
	cState()->SetFillStatus(fraction);
}


void GUI_StatusBar::SetFillStatusByAmount(double amount)
{
	auto s = cState();
	assert(amount >= 0.0 && amount <= s->GetMaxCapacity() && "Passed an invalid amount (< 0 or > capacity)!");
	s->SetFillStatus(amount / s->GetMaxCapacity());
}


void GUI_StatusBar::SetCapacity(double capacity, string unit)
{
	auto s = cState();
	s->SetUnitString(unit);
	s->SetMaxCapacity(capacity);
}


void GUI_StatusBar::SetVerbose(bool verbose)
{
	cState()->SetVerbose(verbose);
}

void GUI_StatusBar::SetUnitScaling(bool scaling)
{
	cState()->SetUnitScaling(scaling);
}

void GUI_StatusBar::createStatusString(string &str)
{
	auto s = cState();
	//write the current status
	if (s->GetUnitScaling())
	{
		//bar is set to unit scaling, create the appropriate string
		str = Helpers::doubleToUnitString(s->GetMaxCapacity() * s->GetFillStatus(), s->GetUnitString());
	}
	else
	{
		//numbers will be drawn in sceintific notation if too large
		str = Helpers::doubleToScientificString(s->GetMaxCapacity() * s->GetFillStatus(), 5, 2) + " " + s->GetUnitString();
	}

	//if verbose output is demanded, append the maximum capacity
	if (s->GetVerbose())
	{
		str += " / ";
		if (s->GetUnitScaling())
		{
			str += Helpers::doubleToUnitString(s->GetMaxCapacity(), s->GetUnitString());
		}
		else
		{
			str += Helpers::doubleToScientificString(s->GetMaxCapacity(), 5, 2) + " " + s->GetUnitString();
		}
	}
}


void GUI_StatusBar::prepareFullStatusString(string &str)
{
	if (statussrf == NULL)
	{
		statussrf = GUI_Draw::createElementBackground(style, width, height * 2);
	}
	oapiBlt(statussrf, src, 0, 0, 0, 0, width, height);
	font->Print(statussrf, str, width / 2, (int)(height / 2), _R(style->CornerRadius(), 0, width - style->CornerRadius(), height),
		true, T_CENTER, V_CENTER);
}


void GUI_StatusBar::prepareEmptyStatusString(string &str)
{
	if (statussrf == NULL)
	{
		statussrf = GUI_Draw::createElementBackground(style, width, height * 2);
	}
	oapiBlt(statussrf, src, 0, height, 0, height, width, height);
	font->Print(statussrf, str, width / 2, (int)(height * 1.5), _R(style->CornerRadius(), height, width - style->CornerRadius(), height * 2),
		false, T_CENTER, V_CENTER);
}

GUI_StatusBarState *GUI_StatusBar::cState()
{
	return (GUI_StatusBarState*)state;
}