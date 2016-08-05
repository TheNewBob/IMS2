#pragma once
#include "IMS_ModuleFunction_Thruster.h"
class IMS_ModuleFunction_Rcs :
	public IMS_ModuleFunction_Thruster
{
public:
	IMS_ModuleFunction_Rcs(IMS_ModuleFunctionData_Rcs *_data, IMS_Module *_module);
	~IMS_ModuleFunction_Rcs();

	virtual void PostLoad(){};
	virtual void AddFunctionToVessel(IMS2 *vessel);
	virtual void RemoveFunctionFromVessel(IMS2 *vessel);
	virtual void AddExhaustsToVessel();
	virtual void RemoveExhaustsFromVessel();

protected:
	THRUSTER_HANDLE mirroredthruster = NULL;

private:
	IMS_ModuleFunctionData_Rcs *data;
};

