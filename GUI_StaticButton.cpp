#include "GUI_Common.h"
#include "GUI_StaticButton.h"


GUI_StaticButton::GUI_StaticButton(RECT _rect, int srcx, int srcy, SURFHANDLE _src, int _id)
	: GUI_BaseElement(_rect, _id, NULL)
{
	src = _src;
	srcRect = _R(srcx, srcy, srcx + width, srcy + height);
}

GUI_StaticButton::~GUI_StaticButton(void)
{
}


void GUI_StaticButton::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
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

bool GUI_StaticButton::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (_event == LBUP)
	{
		return GUI_BaseElement::ProcessMe(_event, _x, _y);
	}
	return false;
}
