#pragma once


class IMS_ModuleFunctionData_Comm :
	public IMS_ModuleFunctionData_Base
{
public:
	friend class IMS_ModuleFunction_Comm;

	IMS_ModuleFunctionData_Comm();
	~IMS_ModuleFunctionData_Comm();

	bool HasGui() { return true; };

private:
	string trackinganimname = "";
	string deployanimname = "";
	string searchanimname = "";

	bool processConfigLine(vector<string> &tokens);
	bool validateData(string configfile);

};

