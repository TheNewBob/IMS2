#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "Events.h"
#include "IMS.h"
#include "GUIentity.h"
#include "GUIplugin.h"
#include "GUImanager.h"
#include "Events.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_InputCallback.h"
#include "GUI_Module.h"
#include "GUI_Looks.h"


GUI_Module::GUI_Module(GUImanager *vesselgui, IMS_Module *_module)
	: GUI_ScrollablePage(setRect(vesselgui), GUI_MODULE_PAGE, _module->GetGui()->GetStyle()), module(_module)
{
	//register the module gui as root element of the plugin
	module->GetGui()->RegisterGuiElement(this, -1);
}



GUI_Module::~GUI_Module()
{
}



RECT GUI_Module::setRect(GUImanager *vesselgui)
{
	//get the parent element, its rect and its style
	GUI_BaseElement *parent = vesselgui->GetElementById(MD_MODULES_CTRLPG);
	GUI_ElementStyle *parentstyle = parent->GetStyle();
//	parent->GetRect(rect);

	//set the element to be exactly inside the drawable rect of its parent
	rect.left = parentstyle->MarginLeft();
	rect.right = parent->GetWidth() - parentstyle->MarginRight();
	rect.top = parentstyle->MarginTop();
	rect.bottom = parent->GetHeight() - parentstyle->MarginBottom();

	width = rect.right - rect.left;
	//set our height to the height of the parent
	height = rect.bottom - rect.top;
	return rect;
}


