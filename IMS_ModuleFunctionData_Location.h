#pragma once
class IMS_ModuleFunctionData_Location :
	public IMS_ModuleFunctionData_Base
{
public:
	IMS_ModuleFunctionData_Location();
	~IMS_ModuleFunctionData_Location();
	double GetVolume();
protected:
	double volume;										//!< usable volume in m^3
	bool validateData(string configfile);
	bool processConfigLine(vector<string> &tokens);

};

