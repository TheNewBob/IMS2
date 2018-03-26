#pragma once

class GUI_ModuleFunction_Pressurised :
	public GUI_ModuleFunction_Base
{
public:
	GUI_ModuleFunction_Pressurised(IMS_ModuleFunction_Pressurised *moduleFunction, GUIplugin *gui);
	~GUI_ModuleFunction_Pressurised();

	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

	void RefreshComponents();

protected:
	IMS_ModuleFunction_Pressurised *function;

	GUI_Label *title = NULL;
	GUI_LabelValuePair *volume = NULL;
	GUI_LabelValuePair *componentMass = NULL;
	GUI_DynamicButton *addComponentBtn = NULL;
	GUI_DynamicButton *removeComponentBtn = NULL;

	IMS_Component_UI *componentsView = NULL;

	//virtual bool updateMe();

};

