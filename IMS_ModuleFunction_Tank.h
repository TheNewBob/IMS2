#pragma once

class IMS_Storable;
class IMS_PropulsionManager;
class GUI_ModuleFunction_Tank;

bool PropellantTypeSelected(int idx, void* tank);

class IMS_ModuleFunction_Tank :
	public IMS_ModuleFunction_Pressurised
{
public:
	IMS_ModuleFunction_Tank(IMS_ModuleFunctionData_Tank *_data, IMS_Module *_module);
	~IMS_ModuleFunction_Tank();

	//overloads from IMS_ModuleFunction_Base
	virtual void PostLoad();
	virtual void SaveState(FILEHANDLE scn);
	virtual void AddFunctionToVessel(IMS2 *vessel);
	virtual void RemoveFunctionFromVessel(IMS2 *vessel);
	virtual GUI_ModuleFunction_Base *GetGui();
	
	/**
	 * \brief Sets the status of the tanks valve
	 * \param open Pass True to open the valve, false to close it
	 */
	void SetValveStatus(bool open);
	
	/**
	 * \return True if the valve on this tank is currently open, false if not
	 */
	bool GetValveStatus();

	/**
	 * \brief Sets the tank to a new propellant type, IF the tank is empty
	 * \param propellant_id the id of the new propellant type
	 */
	void SetPropellantType(int propellant_id);
	
	/**
	 * \return The storable containing the actual propellant in this tank
	 */
	IMS_Storable *GetTank() { return tank; };


protected:

	virtual bool processScenarioLine(string line);
	virtual bool ProcessEvent(Event_Base *e);
	IMS_Storable *tank = NULL;
	IMS_PropulsionManager *propmanager = NULL;
	

private:
	IMS_ModuleFunctionData_Tank *data;
	GUI_ModuleFunction_Tank *menu;


};

