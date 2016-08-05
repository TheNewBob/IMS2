#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "IMS_ModuleDataManager.h"


IMS_ModuleFunctionData_Thruster::IMS_ModuleFunctionData_Thruster()
{
	type = MTYPE_THRUSTER;
}


IMS_ModuleFunctionData_Thruster::~IMS_ModuleFunctionData_Thruster()
{

}

THRUSTERMODE *IMS_ModuleFunctionData_Thruster::GetThrusterMode(int mode)
{
	assert((UINT)mode < thrustermodes.size() && "Trying to retrieve non-existing thrustermode!");
	return &thrustermodes[mode];
}

THRUSTEREXHAUST *IMS_ModuleFunctionData_Thruster::GetThrusterExhaust(int idx)
{
	assert((UINT)idx < exhausts.size() && "Trying to retrieve non-existing exhaust!");
	return &exhausts[idx];
}


bool IMS_ModuleFunctionData_Thruster::processConfigLine(vector<string> &tokens)
{
	//remembers if we're inside a thrustermode or exhaust definition
	static bool readingmode = false;
	static bool readingexhaust = false;

	if (tokens[0] == "begin_thrustermode")
	{
		if (readingmode)
		{
			throw invalid_argument("Missing END_THRUSTERMODE!");
		}
		else if (readingexhaust)
		{
			throw invalid_argument("Cannot declare thrustermode inside exhaust definition!");
		}
		//add a new thrustermode
		THRUSTERMODE newmode;
		newmode.Name = tokens[1];
		thrustermodes.push_back(newmode);
		readingmode = true;
		return true;
	}

	else if (tokens[0] == "end_thrustermode")
	{
		if (!readingmode)
		{
			throw invalid_argument("ERROR: Missing BEGIN_THRUSTERMODE <thrustermode name>");
		}
		readingmode = false;
		return true;
	}

	else if (tokens[0] == "begin_exhaust")
	{
		if (readingexhaust)
		{
			throw invalid_argument("Missing END_EXHAUST!");
		}
		else if (readingmode)
		{
			throw invalid_argument("Cannot declare exhaust inside thrustermode definition!");
		}
		//add a new exhaust
		THRUSTEREXHAUST newexhaust;
		exhausts.push_back(newexhaust);
		readingexhaust = true;
		return true;
	}

	else if (tokens[0] == "end_exhaust")
	{
		if (!readingexhaust)
		{
			throw invalid_argument("ERROR: Missing BEGIN_EXHAUST <thrustermode name>");
		}
		readingexhaust = false;
		return true;
	}

	//read thrustermode properties
	else if (readingmode)
	{
		processMode(tokens);
		return true;
	}

	//read exhaust properties
	else if (readingexhaust)
	{
		processExhaust(tokens);
		return true;
	}

	else if (tokens[0] == "dir")
	{
		//reading thrust direction
		if (tokens.size() != 4)
		{
			throw invalid_argument("ERROR: dir expects [float float float] as parameter for Thruster Modulefunction");
		}
		thrustdirection.x = Helpers::stringToDouble(tokens[1]);
		thrustdirection.y = Helpers::stringToDouble(tokens[2]);
		thrustdirection.z = Helpers::stringToDouble(tokens[3]);
		return true;
	}

	else if (tokens[0] == "pos")
	{
		//reading thruster position
		if (tokens.size() != 4)
		{
			throw invalid_argument("ERROR: pos expects [float float float] as parameter for Thruster Modulefunction");
		}
		thrusterpos.x = Helpers::stringToDouble(tokens[1]);
		thrusterpos.y = Helpers::stringToDouble(tokens[2]);
		thrusterpos.z = Helpers::stringToDouble(tokens[3]);
		return true;
	}


	return false;
}


void IMS_ModuleFunctionData_Thruster::processMode(vector<string> &tokens)
{
	if (tokens[0] == "consumable")
	{
		if (tokens.size() != 3)
		{
			throw invalid_argument("ERROR: Consumable expects name:string ratio:int as parameter for Thruster module function");
		}

		int propellantid = IMS_ModuleDataManager::GetConsumableId(tokens[1]);
		if (propellantid == -1)
		{
			throw invalid_argument("ERROR: consumable not found in consumables.cfg: " + tokens[1]);
		}
		thrustermodes.back().Propellants.push_back(propellantid);
		thrustermodes.back().Ratio.push_back((float)Helpers::stringToDouble(tokens[2].data()));
	}

	else if (tokens[0] == "thrust")
	{
		thrustermodes.back().Thrust = Helpers::stringToDouble(tokens[1]);
	}

	else if (tokens[0] == "isp")
	{
		thrustermodes.back().Isp = Helpers::stringToDouble(tokens[1]);
	}

	else if (tokens[0] == "efficiency")
	{
		thrustermodes.back().Efficiency = Helpers::stringToDouble(tokens[1]);
	}

	else if (tokens[0] == "exhaustlength")
	{
		thrustermodes.back().Exhaustlength = Helpers::stringToDouble(tokens[1]);
	}

	else if (tokens[0] == "exhaustwidth")
	{
		thrustermodes.back().Exhaustwidth = Helpers::stringToDouble(tokens[1]);
	}
}



void IMS_ModuleFunctionData_Thruster::processExhaust(vector<string> &tokens)
{
	if (tokens[0] == "length")
	{
		exhausts.back().length = Helpers::stringToDouble(tokens[1]);
	}
	else if (tokens[0] == "width")
	{
		exhausts.back().width = Helpers::stringToDouble(tokens[1]);
	}
	else if (tokens[0] == "pos")
	{
		if (tokens.size() != 4)
		{
			throw invalid_argument("ERROR: pos expects [float float float] as parameters for Thruster Exhaust");
		}

		exhausts.back().pos = _V(Helpers::stringToDouble(tokens[1]),
								 Helpers::stringToDouble(tokens[2]),
								 Helpers::stringToDouble(tokens[3]));
	}
	else if (tokens[0] == "dir")
	{
		if (tokens.size() != 4)
		{
			throw invalid_argument("ERROR: dir expects [float float float] as parameters for Thruster Exhaust");
		}

		exhausts.back().pos = _V(Helpers::stringToDouble(tokens[1]),
					   			 Helpers::stringToDouble(tokens[2]),
								 Helpers::stringToDouble(tokens[3]));
	}

}



bool IMS_ModuleFunctionData_Thruster::validateData(string configfile)
{
	
	bool isvalid = true;
	try
	{
		validateModes();
		validateExhausts();
	}
	catch (invalid_argument e)
	{
		Helpers::writeToLog(e.what() + configfile, L_ERROR);
	}
	
	if (thrustermodes.size() == 0)
	{
		Helpers::writeToLog(string("No THRUSTERMODE defined for Thruster module function in " + configfile), L_ERROR);
		isvalid = false;
	}

	if (exhausts.size() == 0)
	{
		Helpers::writeToLog(string("No Exhausts defined for Thruster module function in " + configfile), L_ERROR);
		isvalid = false;
	}



	if (thrustdirection.x == -1 &&
		thrustdirection.y == -1 &&
		thrustdirection.z == -1)
	{
		Helpers::writeToLog("No valid dir parameter for Thruster module function in " + configfile, L_ERROR);
		isvalid = false;
	}
	return isvalid;
}





void IMS_ModuleFunctionData_Thruster::validateModes()
{
	for (UINT i = 0; i < thrustermodes.size(); ++i)
	{
		if (thrustermodes[i].Isp == -1)
		{
			stringstream ss;
			ss << "No ISP defined for thruster mode " << i << " (0-based index!) in ";
			throw(invalid_argument(ss.str()));
		}

		if (thrustermodes[i].Thrust == -1)
		{
			stringstream ss;
			ss << "No Thrust defined for thruster mode " << i << " (0-based index!) in ";
			throw(invalid_argument(ss.str()));
		}
	}

}


void IMS_ModuleFunctionData_Thruster::validateExhausts()
{
	for (UINT i = 0; i < exhausts.size(); ++i)
	{
		if (exhausts[i].length == -1)
		{
			stringstream ss;
			ss << "No Length defined for thruster exhaust " << i << " (0-based index!) in ";
			throw(invalid_argument(ss.str()));
		}

		if (exhausts[i].width == -1)
		{
			stringstream ss;
			ss << "No width defined for thruster exhaust " << i << " (0-based index!) in ";
			throw(invalid_argument(ss.str()));
		}

		if (exhausts[i].dir.x == 0 &&
			exhausts[i].dir.y == 0 &&
			exhausts[i].dir.z == 0)
		{
			//no exhaust dir defined, assume opposite of thrust direction
			exhausts[i].dir = thrustdirection * -1;
		}
	}
}
