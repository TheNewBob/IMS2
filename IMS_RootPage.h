#pragma once

/**
 * Base class for root menu pages designed specifically for IMS2.
 * Any page inheriting from this class receives vessel events via the VESSEL_TO_GUI_PIPE.
 */
class IMS_RootPage :
	public GUI_Page, public EventHandler
{
public:
	IMS_RootPage(IMS2 *vessel, int id, RECT mRect, GUI_ElementStyle *style);
	~IMS_RootPage();

	virtual bool Update();
};

