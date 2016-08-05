#pragma once


class IMS_ModuleFunction_Factory
{
public:
	IMS_ModuleFunction_Factory();
	~IMS_ModuleFunction_Factory();

	static IMS_ModuleFunction_Base *CreateNewModuleFunction(IMS_ModuleFunctionData_Base *_data, IMS_Module *module);
	static IMS_ModuleFunctionData_Base *CreateNewModuleFunctionData(FUNCTIONTYPE type);
	static string GetFunctionTypeString(FUNCTIONTYPE type);
	static FUNCTIONTYPE GetFunctionTypeFromString(string identifier);
};

