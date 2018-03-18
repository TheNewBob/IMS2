#pragma once

/**
 * \brief A reusable page that displays properties and components of a modulefunction that is also a location.
 * \Note As modulefunction UIs this element is self-registering and doesn't need to be registered with the GUIentity after creation.
 */
class IMS_Component_UI :
	public GUI_Page
{
public:
	IMS_Component_UI(IMS_ModuleFunction_Location *modFunction, GUIplugin *gui, RECT mRect, int _id, int parent_id, GUI_ElementStyle *_style, bool drawbackground = false);
	virtual ~IMS_Component_UI();

	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);


protected:
	GUI_LabelValuePair *maxVolume = NULL;
	GUI_LabelValuePair *availableVolume = NULL;
	GUI_DynamicButton *addComponentBtn = NULL;
	GUI_DynamicButton *removeComponentBtn = NULL;
	GUI_ListBox *componentList = NULL;
};

