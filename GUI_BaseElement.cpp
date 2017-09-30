#include "GUI_Common.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"

class GUI_BaseElementState;
class GUI_BaseElementResource;

GUI_BaseElement::GUI_BaseElement(RECT _rect, int _id, GUI_ElementStyle *_style)
	: rect(_rect), width(rect.right - rect.left), height(rect.bottom - rect.top),
	id(_id), style(_style), src(NULL), visible(true)
{
	if (style != NULL)
	{
		font = style->GetFont();
	}
}


GUI_BaseElement::~GUI_BaseElement()
{
	revokeState();

	//some elements like pages don't actually allocate a source surface
	if (src != NULL)
	{
		oapiDestroySurface(src);
	}
	//delete all plugins of this element
	for (UINT i = 0; i < plugins.size(); ++i)
	{
		delete plugins[i];
	}
}


void GUI_BaseElement::Draw(SURFHANDLE _tgt, RECT &drawablerect, int xoffset, int yoffset)
{
	if (visible)
	{
		if (!noBlitting)
		{
			DrawMe(_tgt, xoffset, yoffset, drawablerect);
		}
		DrawChildren(_tgt, drawablerect, xoffset, yoffset);
	}
}

int GUI_BaseElement::ProcessMouse(GUI_MOUSE_EVENT _event, int _x, int _y)
//returns the ID of the triggered element
{
	if (!visible) return -1;

	//converting coordinates to coordinates relative to this element
	int relativeX = _x - rect.left;
	int relativeY = _y - rect.top;

	//checking if any of the children got clicked
	int eventId = ProcessChildren(_event, relativeX, relativeY);

	//checking if this element has been clicked 
	if (eventId == -1 && ProcessMe(_event, _x, _y))
	{
		return id;
	}
	return eventId;
}



void GUI_BaseElement::GetRect(RECT &mRect)
{
	mRect = rect;
}


GUI_ELEMENT_TYPE GUI_BaseElement::GetType()
{
	return type;
}

void GUI_BaseElement::DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect)
{

}

void GUI_BaseElement::DrawChildren(SURFHANDLE _tgt, RECT &drawablerect, int xoffset, int yoffset)
{
	//calculating offset for children to so they draw at proper positions on the screen
	int newxOffset = xoffset + rect.left;
	int newyOffset = yoffset + rect.top;

	//get the real drawable rect of this element
	RECT mydrawablerect;
	calculateMyDrawableRect(drawablerect, xoffset + rect.left, yoffset + rect.top, mydrawablerect);

	for (UINT i = 0; i < children.size(); ++i)
	{
		children[i]->Draw(_tgt, mydrawablerect, newxOffset, newyOffset);
	}

	//draw plugins
	for (UINT i = 0; i < plugins.size(); ++i)
	{
		plugins[i]->DrawChildren(_tgt, mydrawablerect, newxOffset, newyOffset);
	}
}


void GUI_BaseElement::SetStyle(GUI_ElementStyle *style)
{
	if (style != this->style)
	{
		if (src != NULL)
		{
			oapiDestroySurface(src);
			src = NULL;
		}
		
		this->style = style;
		font = this->style->GetFont();
		createResources();
		updatenextframe = true;
	}
}

void GUI_BaseElement::SetStyleSetForPlugins(string styleset)
{
	for (auto i = plugins.begin(); i != plugins.end(); ++i)
	{
		(*i)->SetStyleset(styleset);
	}
}



bool GUI_BaseElement::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//use this function for declaring custom event handling in inherited classes
	if (_x > rect.left && _x < rect.right && _y > rect.top && _y < rect.bottom)
	{
		return true;
	}
	return false;
}



int GUI_BaseElement::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//check the plugins first
	for (UINT i = plugins.size(); i > 0; --i)
	{
		//return own id if any plugins of this consumed the event to trigger redraw
		if (plugins[i - 1]->ProcessChildren(_event, _x, _y))
		{
			return id;
		}
	}

	for (UINT i = children.size(); i > 0; --i)
	{
		int eventId = children[i - 1]->ProcessMouse(_event, _x, _y);
		if (eventId != -1)
		{
			return eventId;
		}
	}

	return -1;
}

bool GUI_BaseElement::Update()
{
	bool needsupdate = false;
	//first check if the element i visible at all
	if (visible)
	{
		//check if this element needs a redraw
		if (updateMe())
		{
			needsupdate = true;
		}
		if (updateChildren())
		{
			needsupdate = true;
		}
	}
	return needsupdate;
}


bool GUI_BaseElement::updateChildren()
{
	bool needsupdate = false;
	for (UINT i = 0; i < children.size(); ++i)
	{
		if (children[i]->Update())
		{
			needsupdate = true;
		}
	}

	for (UINT i = 0; i < plugins.size(); ++i)
	{
		if (plugins[i]->Update())
		{
			needsupdate = true;
		}
	}
	return needsupdate;
}


int GUI_BaseElement::GetId()
{
	return id;
}

void GUI_BaseElement::AddChild(GUI_BaseElement *child)
{
	children.push_back(child);
}

void GUI_BaseElement::SetVisible(bool _visible)
{
	visible = _visible;
}

bool GUI_BaseElement::GetVisible()
{
	return visible;
}



void GUI_BaseElement::AddPlugin(GUIplugin *plugin)
{
	plugins.push_back(plugin);
}


void GUI_BaseElement::RemovePlugin(GUIplugin *plugin)
{
	assert(find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	vector<GUIplugin*>::iterator i = find(plugins.begin(), plugins.end(), plugin);
	plugins.erase(i);
}



void GUI_BaseElement::ShareStateWith(GUI_BaseElement *who)
{
	state->ShareWith(who);
}


void GUI_BaseElement::CancelStateSharingWith(GUI_BaseElement *who)
{
	state->CancelSharingWith(this);
	revokeState();
}


void GUI_BaseElement::revokeState()
{
	if (state != NULL) 
	{
		// destroy the state if it is owned by the calling element, otherwise just don't reference it anymore.
		if (state->IsOwnedBy(this))
		{
			delete state;
		}
		else
		{
			state->CancelSharingWith(this);
		}

		state = NULL;
	}
}

void GUI_BaseElement::setState(GUI_BaseElementState *state)
{
	if (state != NULL)
	{
		revokeState();
	}
	this->state = state;
}

void GUI_BaseElement::swapState(GUI_BaseElementState *newstate)
{
	assert(newstate->sharers.size() == 0 && "A state that is already shared cannot replace another state!");
	newstate->owner = this;

	// if this element doesn't have a state yet, we don't need to worry about sharers.
	if (state != NULL)
	{
		newstate->owner = this;
		for (UINT i = state->sharers.size(); i > 0; --i)
		{
			GUI_BaseElement *sharer = state->sharers[i];
			newstate->ShareWith(sharer);
			state->CancelSharingWith(sharer);
		}
	}
	
	setState(newstate);
}

void GUI_BaseElement::calculateBlitData(int xoffset, int yoffset, RECT &drawablerect, BLITDATA &OUT_blitdata)
{
	//first, check if the element is inside the drawable rectangle of its parent at all
	if (xoffset > drawablerect.right ||
		xoffset + width < drawablerect.left ||
		yoffset > drawablerect.bottom ||
		yoffset + height < drawablerect.top)
	{
		return;
	}

	//the target position is either where the element is supposed to be drawn,
	//or the beginning of the drawable rect if the position is outside the drawable rect
	OUT_blitdata.tgtrect.left = max(xoffset, drawablerect.left);
	OUT_blitdata.tgtrect.top = max(yoffset, drawablerect.top);

	//the dimensions of the blitting rectangle are either the element's dimensions,
	//or the remaining space in the drwable rectangle
	
	//first, calculate the maximum dimensions the element can have after we calculated its top left position
	int maxheight = min(height, height - (OUT_blitdata.tgtrect.top - yoffset));
	
	//next, see if those dimensions overlap the drawable rect at their opposite side
	int bottomposition = OUT_blitdata.tgtrect.top + maxheight;

	if (bottomposition > drawablerect.bottom)
	{
		maxheight -= (bottomposition - drawablerect.bottom);
	}

	OUT_blitdata.width = width;
	OUT_blitdata.height = maxheight;
	OUT_blitdata.tgtrect.right = OUT_blitdata.tgtrect.left + OUT_blitdata.width;
	OUT_blitdata.tgtrect.bottom = OUT_blitdata.tgtrect.top + OUT_blitdata.height;

	//the source rectangle starts at 0,0. But if the image was cropped on the left top,
	//we need to move it. The bottom right will be automatically cropped by width and height
	OUT_blitdata.srcrect.left = max(0, drawablerect.left - xoffset);
	OUT_blitdata.srcrect.top = max(0, drawablerect.top - yoffset);
	OUT_blitdata.srcrect.right = OUT_blitdata.srcrect.left + OUT_blitdata.width;
	OUT_blitdata.srcrect.bottom = OUT_blitdata.srcrect.top + OUT_blitdata.height;
/*	OUT_blitdata.srcrect.left = 0;
	OUT_blitdata.srcrect.top = 0;
	OUT_blitdata.srcrect.right = OUT_blitdata.width;
	OUT_blitdata.srcrect.bottom = OUT_blitdata.height;*/

	//TODO: Something's really weird here...

}


void GUI_BaseElement::calculateMyDrawableRect(RECT &drawablerect_parent, int xoffset, int yoffset, RECT &OUT_mydrawablerect)
{
	//first, create the elements drawable rect without taking the parent into account
	OUT_mydrawablerect = _R(xoffset + style->MarginLeft(),
		yoffset + style->MarginTop(),
		xoffset + width - style->MarginRight(),
		yoffset + height - style->MarginBottom());

	//check where the rect is outside the parent and adjust
	if (OUT_mydrawablerect.left < drawablerect_parent.left)
	{
		OUT_mydrawablerect.left = drawablerect_parent.left;
	}
	if (OUT_mydrawablerect.right > drawablerect_parent.right)
	{
		OUT_mydrawablerect.right = drawablerect_parent.right;
	}
	if (OUT_mydrawablerect.top < drawablerect_parent.top)
	{
		OUT_mydrawablerect.top = drawablerect_parent.top;
	}
	if (OUT_mydrawablerect.bottom > drawablerect_parent.bottom)
	{
		OUT_mydrawablerect.bottom = drawablerect_parent.bottom;
	}

	//if the rect is completely outside the parent rect in one axis, 
	//that axis will have a dimension of zero.
	if (OUT_mydrawablerect.left > OUT_mydrawablerect.right)
	{
		OUT_mydrawablerect.right = OUT_mydrawablerect.left;
	}
	if (OUT_mydrawablerect.top > OUT_mydrawablerect.bottom)
	{
		OUT_mydrawablerect.bottom = OUT_mydrawablerect.top;
	}

}


