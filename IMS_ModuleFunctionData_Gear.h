#pragma once


class IMS_ModuleFunctionData_Gear :
	public IMS_ModuleFunctionData_Base
{
	friend class IMS_ModuleFunction_Gear;
public:
	IMS_ModuleFunctionData_Gear();
	~IMS_ModuleFunctionData_Gear();

	bool HasGui() { return false; };

private:
	string deployanimname = "";
	VECTOR3 touchdownpoint = _V(NAN, NAN, NAN);
	VECTOR3 tddir = _V(0, 0, -1);

	bool processConfigLine(vector<string> &tokens);
	bool validateData(string configfile);
	bool candeploy = false;
};

