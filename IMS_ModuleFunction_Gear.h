#pragma once

class IMS_TouchdownPointManager;

/**
 * The different states the gear can have
 */
enum GEARSTATE
{
	RETRACTED,
	DEPLOYING,
	DEPLOYED,
	RETRACTING
};


class IMS_ModuleFunction_Gear :
	public IMS_ModuleFunction_Base
{
public:
	IMS_ModuleFunction_Gear(IMS_ModuleFunctionData_Gear *_data, IMS_Module *_module);
	~IMS_ModuleFunction_Gear();

	virtual void PostLoad();
	virtual void SaveState(FILEHANDLE scn);
	virtual void AddFunctionToVessel(IMS2 *vessel);
	virtual void RemoveFunctionFromVessel(IMS2 *vessel);
	virtual void PreStep(double simdt, IMS2 *vessel);
	virtual bool processScenarioLine(string line);
	virtual IMS_Module *GetModule();

protected:
	bool geardown = false;
	virtual bool ProcessEvent(Event_Base *e);
	StateMachine state;
	IMS_TouchdownPointManager *tdpmanager;
	UINT tdpoint_id = 0;						//!< Stores the id of the touchdown point assigned by the manager.
	/**
	 * \brief Adds the touchdown point to the vessel.
	 * \note Save to call even if point was already added. The method will do nothing in this case.
	 */
	void addTdPointToVessel();

	/**
	 * \brief removes a touchdown point from the vessel
	 * \note Save to call even if the point wasn't added. The method will do nothing in this case.
	 */
	void removeTdPointFromVessel();

private:
	IMS_ModuleFunctionData_Gear *data;

};

