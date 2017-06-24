#pragma once
#include "GUI_Page.h"


struct LAYOUTCOLLECTION;

class GUI_LayoutPage :
	public GUI_Page
{
public:
	GUI_LayoutPage(RECT mRect, int _id, GUI_ElementStyle *_style);
	virtual ~GUI_LayoutPage();

protected:
	/**
	 * \brief override this function to define the layout identifiers for the pages children,
	 *	and map the respective GUI elements to them so they can get placed in the layout on creation.
	 */
	virtual void mapLayoutIdentifiers() = 0;

	/**
	 * \brief Returns the rect (position and dimension) of the element with the corresponding layout id.
	 * \param elementid The string identifier that assigns the element its place in the layout. NOT the id of the gui-element!
	 */
	RECT getElementRect(string elementid);				
	
	map<string, GUI_BaseElement**> layoutids;			//!< Maps string ids used in the layout to their respective GUI elements.
	LAYOUTCOLLECTION *layouts = NULL;
};

