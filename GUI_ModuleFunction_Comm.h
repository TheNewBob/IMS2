#pragma once


enum CHECKBOXSTATE
{
	OFF,
	ON,
	BLINKING
};

class GUI_ModuleFunction_Comm :
	public GUI_ModuleFunction_Base
{
public:
	GUI_ModuleFunction_Comm(IMS_ModuleFunction_Comm *_function, GUIplugin *gui, bool hasdeployment, bool hasscanning, bool hastracking);
	~GUI_ModuleFunction_Comm();

	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

	void SetDeployBoxState(CHECKBOXSTATE newstate);
	void SetSearchBoxState(CHECKBOXSTATE newstate);
	void SetTrackBoxState(CHECKBOXSTATE newstate);
	void SetStateDescription(string state, bool hilighted = false);
	void SetTargetDescription(string target, bool hilighted = false);

	void SetVisualState(int commstate);


protected:

	IMS_ModuleFunction_Comm *function;
	GUI_CheckBox *deploybox = NULL;
	GUI_CheckBox *searchbox = NULL;
	GUI_CheckBox *trackbox = NULL;
	GUI_LabelValuePair *statedescription;
	GUI_LabelValuePair *targetdescription = NULL;
	GUI_DynamicButton *settargetbtn = NULL;

	//flags for animating the checkboxes
	CHECKBOXSTATE deployboxstate = OFF;
	CHECKBOXSTATE searchboxstate = OFF;
	CHECKBOXSTATE trackboxstate = OFF;

	virtual bool updateMe();

private:
	/* returns the required height of this element based on the controls it needs to support */
	int getHeight(bool hasdeployment, bool hasscanning, bool hastracking, int width);
	
	/**
	 * \brief constructs a vector with the ignored fields for this layout.
	 */
	void makeIgnoredFields(vector<string> &OUT_ignored_fields, bool hasdeployment, bool hasscanning, bool hastracking);


};

