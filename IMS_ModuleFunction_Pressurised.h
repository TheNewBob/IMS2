#pragma once



class IMS2;


class IMS_ModuleFunction_Pressurised : public IMS_ModuleFunction_Location
{
public:
	IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module);

	/**
	 * \brief constructor to be used by subclasses
	 */
	IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> locationContexts);


	virtual ~IMS_ModuleFunction_Pressurised();

	virtual void PostLoad(){};
	virtual void SaveState(FILEHANDLE scn){};
	virtual void AddFunctionToVessel(IMS2 *vessel) {};
	virtual void RemoveFunctionFromVessel(IMS2 *vessel) {};
	virtual bool processScenarioLine(string line){ return false; };
	// TODO: Return UI
	virtual double GetMass();
	virtual IMS_Module *GetModule();


protected:
	virtual bool ProcessEvent(Event_Base *e);			


private:
	IMS_ModuleFunctionData_Pressurised *data;			
};