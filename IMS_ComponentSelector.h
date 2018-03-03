#pragma once

struct IMS_Location;

class IMS_ComponentSelector :
	public GUIpopup
{
public:
	IMS_ComponentSelector(vector<IMS_Component_Model_Base*> components, IMS_Location *location, GUI_BaseElement *parent,
		RECT rect, string styleset, std::function<bool(vector<IMS_Component_Model_Base*>)> callback);
	~IMS_ComponentSelector();

protected:
	bool ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

private:
	GUI_ListBox *list;
	GUI_DynamicButton *okbtn;
	GUI_DynamicButton *cancelbtn;

	std::function<bool(vector<IMS_Component_Model_Base*>)> callback;
	vector<IMS_Component_Model_Base*> data;
};

