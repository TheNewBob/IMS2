#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "IMS_ModuleFunctionData_Rcs.h"
#include "IMS_ModuleDataManager.h"


IMS_ModuleFunctionData_Rcs::IMS_ModuleFunctionData_Rcs()
{
	type = MTYPE_RCS;
}


IMS_ModuleFunctionData_Rcs::~IMS_ModuleFunctionData_Rcs()
{
}


bool IMS_ModuleFunctionData_Rcs::processConfigLine(vector<string> &tokens)
{
	if (!IMS_ModuleFunctionData_Thruster::processConfigLine(tokens))
	{ 
		if (tokens[0] == "mirror")
		{
			//this is a mirrored thruster
			mirrored = (bool)Helpers::stringToInt(tokens[1]);
			return true;
		}
	}
	else
	{
		return true;
	}
	return false;
}


bool IMS_ModuleFunctionData_Rcs::validateData(string configfile)
{
	bool thrustervalid = IMS_ModuleFunctionData_Thruster::validateData(configfile);

	if (thrustermodes.size() > 1)
	{
		Helpers::writeToLog(string("More than one thruster mode defined in " + configfile + ". RCS cannot use more than one mode!"), L_WARNING);
	}

	//if the thruster is mirrored, create mirrored exhausts
	if (mirrored)
	{
		mirrorExhausts();
	}

	return thrustervalid;
}


void IMS_ModuleFunctionData_Rcs::mirrorExhausts()
{
	UINT numexhausts = exhausts.size();
	for (UINT i = 0; i < numexhausts; ++i)
	{
		//copy the exhaust data and mirror it
		THRUSTEREXHAUST exh(exhausts[i]);
		exh.pos *= -1;
		exh.dir *= -1;
		exhausts.push_back(exh);
	}
}