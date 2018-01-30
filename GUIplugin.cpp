#include "GUI_Common.h"
#include "GUI_Surface_Base.h"
#include "GUI_Surface.h"
#include "GUI_elements.h"
#include "GUI_Looks.h"
#include "GUIentity.h"
#include "GUIplugin.h"


GUIplugin::GUIplugin()
{
}


GUIplugin::~GUIplugin()
{
}


GUI_BaseElement *GUIplugin::GetFirstVisibleChild()
{
	for (UINT i = 0; i < rootelements.size(); ++i)
	{
		if (rootelements[i]->GetVisible())
		{
			return rootelements[i];
		}
	}
	return NULL;
}


bool GUIplugin::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (isvisible)
	{
		for (UINT i = rootelements.size(); i > 0; --i)
		{
			//return true if a child has consumed the event
			if (rootelements[i - 1]->ProcessMouse(_event, _x, _y) != -1)
			{
				return true;
			}
		}
	}
	return false;
}


bool GUIplugin::Update()
{
	bool needsupdate = false;
	if (isvisible)
	{
		for (UINT i = 0; i < rootelements.size(); ++i)
		{
			if (rootelements[i]->Update())
			{
				needsupdate = true;
			}
		}
	}
	return needsupdate;
}



void GUIplugin::DrawChildren(SURFHANDLE _tgt, RECT &drawablerect, int xoffset, int yoffset)
{
	if (isvisible)
	{
		for (UINT i = 0; i < rootelements.size(); ++i)
		{
			rootelements[i]->Draw(_tgt, drawablerect, xoffset, yoffset);
		}
	}
}



bool GUIplugin::AddChildToParent(GUI_BaseElement *child, int parentId)
{
	if (parentId == -1)
	{
		rootelements.push_back(child);
	}
	else
	{
		Helpers::assertThat([this, parentId]() { return elements.find(parentId) != elements.end(); }, "you're trying to add a child to a non-existant parent!");
		elements[parentId]->AddChild(child);
	}
	return true;
}