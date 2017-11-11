#pragma once


/**
 * Base class for popup dialogs
 *
 * A Popup is a pre-fabricated plugin that can be easily spawned on top
 * of any GUI element. It can be passed a function pointer that will be invoked 
 * with data appropriate for the respective popup when certain actions in the 
 * popup are performed (see documentation of popup implementations).
 * Popups are also designed to be "fire and forgett", i.e. the popup will clean
 * itself up when it gets closed.
 */
class GUIpopup :
	public GUIplugin
{
protected:
	static const int POPUP = 1;					//!< identifier for the page on which the popups controlls will be drawn
	int width;
	int height;

	GUIpopup(GUI_BaseElement *parent, RECT rect, string styleid, string styleset);
	virtual ~GUIpopup();


	/**
	 * \brief closes and destroys the popup
	 */
	void close();
	
};

