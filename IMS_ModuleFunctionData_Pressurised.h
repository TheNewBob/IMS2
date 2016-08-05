#pragma once
//#include "IMS_General_ModuleData.h"


class IMS_ModuleFunctionData_Pressurised :
	public IMS_ModuleFunctionData_Base
{
public:
	IMS_ModuleFunctionData_Pressurised();
	~IMS_ModuleFunctionData_Pressurised();
	double getVolume();
protected:
	double volume;										//!< usable volume in m^3
	bool validateData(string configfile);
	bool processConfigLine(vector<string> &tokens);

};

