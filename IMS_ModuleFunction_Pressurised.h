#pragma once



class IMS2;


class IMS_ModuleFunction_Pressurised : public IMS_ModuleFunction_Base, public IMS_Location
{
public:
	IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module);

	/**
	 * \brief constructor to be used by subclasses
	 */
	IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module, FUNCTIONTYPE _type);


	virtual ~IMS_ModuleFunction_Pressurised();

	virtual void PostLoad(){};
	virtual void SaveState(FILEHANDLE scn){};
	virtual void AddFunctionToVessel(IMS2 *vessel) {};
	virtual void RemoveFunctionFromVessel(IMS2 *vessel) {};
	virtual void PreStep(double simdt, IMS2 *vessel);
	virtual bool processScenarioLine(string line){ return false; };
	virtual double GetMass();
	virtual IMS_Module *GetModule();


protected:
	double volume;										//!< The available volume inside the pressurised space, in m^3
	virtual bool ProcessEvent(Event_Base *e);			


private:
	IMS_ModuleFunctionData_Pressurised *data;			
};