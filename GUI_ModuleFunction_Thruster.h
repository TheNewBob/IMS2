

class GUI_ModuleFunction_Thruster :
	public GUI_ModuleFunction_Base
{
public:
	/**
	 * \param thruster The thruster object this GUI-element controls
	 * \param gui The GUIplugin that is to contain this gui element
	 * \param initialcheckbox The initially selected checkbox for the thrustergroup. Valid values are 0 through 4 (corresponding to group 1, 2, 3, 4, none)
	 * \note Setting the initial checkbox does NOT affect the actual group of the thruster. It's merely so the GUI knows it's initial state!
	 */
	GUI_ModuleFunction_Thruster(IMS_ModuleFunction_Thruster *thruster, GUIplugin *gui);
	~GUI_ModuleFunction_Thruster();

	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
	
	/**
	 * Sets the active option in the group selection radio button
	 * \param index of the option to set(0 through 4)/
	 */
	void SetThGroupRadioBtn(int setbtn);

	/**
	 * Sets the selected thrustermode
	 * \param thmode index of the thrustermode (0 through n-modes - 1)
	 */
	void SetThModeSelection(int setmode);

protected:
	
	vector<GUI_CheckBox> thrustermodes;

	/**
	 * \brief Calculates the height of the GUI widget.
	 * \param numthrustermodes The number of thrustermodes the thruster has
	 */
	int getHeight(int numthrustermodes);

	GUI_ListBox *modesbox = NULL;


private:
	IMS_ModuleFunction_Thruster *basethruster;
	vector<GUI_RadioButton*> groupbuttons;

};