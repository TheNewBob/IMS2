#pragma once
class IMS_ModuleFunction_Tank;

class GUI_ModuleFunction_Tank :
	public GUI_ModuleFunction_Base
{
public:
	GUI_ModuleFunction_Tank(IMS_ModuleFunction_Tank *tank, GUIplugin *gui);
	~GUI_ModuleFunction_Tank();

	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
	 * \brief initialises the guis state once the module function has actually created the tank
	 */
	void Init();
	
	/**
	 * \brief Sets the status of the checkbox controlling the propellant valve
	 * \param checked Pass true to check the checkbox, false to uncheck it
	 */
	void SetValveBox(bool checked);

private:
	IMS_ModuleFunction_Tank *tank;	//!< Pointer to the tank this gui represents
	double currentcontent;			//!< The content currently in the storable, to recognise changes
	GUI_StatusBar *status;			//!< pointer to the status bar showing the tank level
	GUI_CheckBox *valve;			//!< checkbox to open the propellant valve
	GUI_Label *proplabel;			//!< label that displays the currently contained propellant
	GUI_DynamicButton *proptypebtn; //!< button to select the propellant type (only visible when tank empty)
	GUI_DynamicButton *cheatbutton = NULL;			//!< cheat button to fill up the tank. Only visible in debug builds.
	bool updatenextframe = false;
	bool updateMe();
};

