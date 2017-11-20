#include "GUI_Common.h"
#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "GUI_Page.h"
#include "IMS_RootPage.h"


IMS_RootPage::IMS_RootPage(IMS2 *vessel, int id, RECT mRect, GUI_ElementStyle *style)
	: GUI_Page(mRect, id, style, true)
{
	// connect the event handler to the vessel. 
	// Since root elements cannot be moved, there's no need to provide for further coupling and decoupling.
	vessel->ConnectToMyEventGenerator(createEventSink(VESSEL_TO_GUI_PIPE), 
									  VESSEL_TO_GUI_PIPE);
	eventhandlertype = GUISURFACE_HANDLER;
}


IMS_RootPage::~IMS_RootPage()
{
}


bool IMS_RootPage::Update()
{
	processWaitingQueue();
	sendEvents();
	return GUI_BaseElement::Update();
}