#pragma once



class IMS2;
class GUI_ModuleFunction_Pressurised;

class IMS_ModuleFunction_Pressurised : public IMS_ModuleFunction_Location
{
	friend class GUI_ModuleFunction_Pressurised;
public:
	IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module);

	/**
	 * \brief constructor to be used by subclasses
	 */
	IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> locationContexts, bool createGui = false);


	virtual ~IMS_ModuleFunction_Pressurised();

	virtual void PostLoad(){};
	virtual void AddFunctionToVessel(IMS2 *vessel) {};
	virtual void RemoveFunctionFromVessel(IMS2 *vessel) {};
	virtual GUI_ModuleFunction_Base *GetGui();
	virtual double GetMass();
	virtual IMS_Module *GetModule();


protected:
	virtual bool ProcessEvent(Event_Base *e);			


private:
	IMS_ModuleFunctionData_Pressurised *data;
	GUI_ModuleFunction_Pressurised *menu = NULL;
};