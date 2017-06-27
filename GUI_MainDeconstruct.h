#pragma once


//this class handles the deconstruction menu
typedef enum
{
	X_AXIS, Y_AXIS, Z_AXIS
}SORT_MODULE_BY;

typedef enum
{
	CORE, TRIGGER
}CONNECTING_TO;


class GUI_MainDeconstruct :
	public GUI_Page
{
public:
	GUI_MainDeconstruct(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel);
	~GUI_MainDeconstruct();
	void SetVisible(bool _visible);
	void UpdateModulesList(SORT_MODULE_BY sort_by = Z_AXIS, bool symmetric = false);

private:
	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
	void selectModules(int selected_index);
	void selectModulesConnectingTo(IMS_Module *module, vector<IMS_Module*> &checkedmodules, double triggerdistance, VECTOR3 &corepos, CONNECTING_TO connecting_to);
	inline double getAxisDistanceBetween(VECTOR3 a, VECTOR3 b);
	
	/* returns true if axis distance in sorting_order a to b is equal or larger than triggerdistance 
	 * in explicit, this means that a is eligible for a split perpendicular to sorted_axis at triggerdistance
	 * if symmetry_box is checked, this function will compare absolute values (without sign).
	 * a: the position of the tested module
	 * b: the position of the CORE module
	 * triggerdistance: The axis-distance between TRIGGER and CORE
	 */
	bool isBeyondTriggerdistance(VECTOR3 a, VECTOR3 b, double triggerdistance);

	SORT_MODULE_BY sorting_order = Z_AXIS;
	IMS2 *vessel;
	GUI_ListBox *modules_list;
	GUI_ListBox *selected_modules_list;
	GUI_CheckBox *symmetry_box;
	vector<IMS_Module*> modules;
	vector<IMS_Module*> sortedmodules;
	vector<IMS_Module*> selected_modules;
};

