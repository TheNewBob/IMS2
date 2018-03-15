#pragma once
class IMS_Component_UI :
	public GUI_Page
{
public:
	IMS_Component_UI(IMS_ModuleFunction_Location *modFunction, GUIplugin *gui, RECT mRect, int _id, GUI_ElementStyle *_style, bool drawbackground = false);
	virtual ~IMS_Component_UI();

protected:
	GUI_LabelValuePair *maxVolume = NULL;
	GUI_LabelValuePair *availableVolume = NULL;
	GUI_DynamicButton *addComponentBtn = NULL;
	GUI_DynamicButton *removeComponentBtn = NULL;
	GUI_ListBox *componentList = NULL;
};

