#pragma once
#include "Common.h"
#include "GUI_Common.h"
#include "GUI_BaseElement.h"
#include "GUI_Page.h"
#include "GUI_Surface_Base.h"
#include "GUI_Surface.h"


/**
 * An IMS2 specific implementation of a GUI_Surface.
 */
class IMS2_GUI_Surface :
	public GUI_Surface
{
public:
	IMS2_GUI_Surface(IMS2 *vessel, int panelid, GUI_Page *rootelement);
	~IMS2_GUI_Surface();

};

