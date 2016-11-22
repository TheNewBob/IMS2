#pragma once

class GUI_ModuleFunction_Thruster;
class IMS_PropulsionManager;

/**
 * \brief Modulefunction that defines a basic thruster.
 * 
 * This class is not abstract, but might become in the future. It is intended that 
 * specialised thruster types or roles are derived from this class (such as chemical thrusters,
 * electrical, etc). 
 * The thruster modulefunction provides implementation for different and selectable thrustermodes,
 * used propellant types and ratios as well as exhaust rendering.
 */
class IMS_ModuleFunction_Thruster :
	public IMS_ModuleFunction_Base
{
public:
	IMS_ModuleFunction_Thruster(IMS_ModuleFunctionData_Thruster *_data, IMS_Module *_module, bool creategui);
	virtual ~IMS_ModuleFunction_Thruster();

	virtual void PostLoad();
	virtual void SaveState(FILEHANDLE scn);
	virtual void AddFunctionToVessel(IMS2 *vessel);
	virtual void RemoveFunctionFromVessel(IMS2 *vessel);
	virtual void PreStep(double simdt, IMS2 *vessel);

	virtual GUI_ModuleFunction_Base *GetGui();

	/**
	 * \brief adds all exhausts of the thruster to the vessel
	 */
	virtual void AddExhaustsToVessel();
	
	/**
	* \brief Removes all exhausts of the thruster from the vessel
	*/
	virtual void RemoveExhaustsFromVessel();

	/**
	 * \brief sets the thruster mode of the thruster.
	 * \param modeidx The index the desired mode has in the thrusters modes list.
	 */
	virtual void SetThrusterMode(int modeidx);
	
	/**
	 * \brief Sets the thrusters control group
	 * \param thrustergroup The value of a THGROUP_TYPE identifier, or -1 if the thruster is removed from all groups
	 */
	virtual void SetThrusterGroup(int thrustergroup);

	IMS_ModuleFunctionData_Thruster *GetData() { return data; };


protected:
	virtual bool ProcessEvent(Event_Base *e);

	/**
	* \return A vector with the thrusters exhausts with vessel-relative position and direction
	*/
	vector<THRUSTEREXHAUST> getRelativeExhausts();

	THRUSTER_HANDLE thruster = NULL;
	int currentthrustermode = 0;						//!< The index of the currently active thruster mode
	bool operational = true;							//!< Shows whether the thruster is currentl yoperational or not
	int thrustergroup = -1;								//!< Contains the THGROUP_TYPE value or -1 if the thruster is not in a group
	virtual bool processScenarioLine(string line);
	IMS_PropulsionManager *propmanager = NULL;
	vector<UINT> exhaust_ids;							//!< Contains the orbiter indices of currently active exhausts for this thruster

private:
	IMS_ModuleFunctionData_Thruster *data;
	GUI_ModuleFunction_Thruster *menu;
};

