#pragma once

/**
 * \brief Default module GUI element.
 *
 * Displays a list of modulefunctions and their respective controls when selected
 */
class GUI_Module :
	public GUI_ScrollablePage
{
public:
	/**
	 * Constructor.
	 * \param vesselgui The GUImanager instance of the vessel containing the module containing this GUI-element
	 * \param _module The IMS_Module instance containing this GUI-element
	*/
	GUI_Module(GUImanager *vesselgui, IMS_Module *_module);
	~GUI_Module();


protected:
	IMS_Module *module;																//!< The IMS_Module instance containing this GUI-element
	map<int, IMS_ModuleFunction_Base*> modulefunctions;								//!< Maps the gui UID of an element to the modulefunction it represents
	
	/**
	 * Sets the rectangle of this GUI to fit inside the parent.
	 * Must be called before the SURFHANDLE for this GUI-element is created,
	 * which means it must be called as a constructor argument.
	 * \param vesselgui The GUImanager instance of the vessel containing this module containing this GUI-element
	 */
	RECT setRect(GUImanager *vesselgui);
};

