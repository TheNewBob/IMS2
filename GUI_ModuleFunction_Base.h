#pragma once


/** 
 * \brief Abstract base class for the user interface of a module function.
 * 
 * Handles the integration and registration into the vessel's GUI. It also
 * handles horizontal sizing. 
 */ 
class GUI_ModuleFunction_Base :
	public GUI_Page
{
public:
	/**
	 * \brief creates and registers a new modulefuncion gui.
	 * @param _height The height of the module function GUI-element
	 * @param gui the GUIplugin object at which the element should be registered
	 * @param _style The skin style of the element
	 * \note gui: Call Module->GetGui() in your ModuleFunction object to receive the propper element
	 */
	GUI_ModuleFunction_Base(int _height, GUIplugin *gui, GUI_ElementStyle *_style)
		: GUI_Page(adjustRectToParent(gui, INT_MAX, _height), -1, _style, true)
	{
		//sorry for the initialiser wizzardry. Basically, the thing has to adjust its width
		//to the parent, it has to do it before the constructor in the parent class is called,
		//and it would be a pain to do the operation on the outside and pass it to the constructor.
		gui->RegisterGuiElement(this, INT_MAX);
	};

	~GUI_ModuleFunction_Base(){};

	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
	{
		return GUI_BaseElement::ProcessChildren(_event, _x, _y);
	};

	/**
	 * \brief positions the element in the y axis. 
	 * @param ypos the new y position of the element
	 * \note This is called from IMS_Module and should never
	 *	be called from anywhere else. 
	 */
	void SetYpos(int ypos)
	{
		rect.top = ypos;
		rect.bottom = ypos + height;
	};

private:

	/**
	 * \brief sets the width of the element to the width of its parent
	 */
	RECT adjustRectToParent(GUIplugin *gui, int parentid, int _height)
	{
		//set my rect to the rect of the parent, so we have proper width
		//the yposition will be set by SetYpos() further down
		RECT retrect;
		gui->GetElementById(parentid)->GetRect(retrect);
		retrect.top = 0;
		retrect.bottom = retrect.top + _height;
		retrect.left += 10;
		retrect.right -= 30;
		return retrect;
	}


};

