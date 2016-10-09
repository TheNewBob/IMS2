#include "Common.h"
#include "ModuleTypes.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include <deque>
#include "SimpleTreeNode.h"
#include "SimplePathNode.h"
#include "StateNode.h"
#include "StateMachine.h"
#include "IMS_Location.h"
#include "IMS_Module.h"

#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_ModuleFunctionData_Animation.h"
#include "IMS_ModuleFunctionData_Comm.h"
#include "IMS_ModuleFunctionData_Tank.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "IMS_ModuleFunctionData_Rcs.h"

#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "IMS_ModuleFunction_Animation.h"
#include "IMS_ModuleFunction_Comm.h"
#include "IMS_Modulefunction_Tank.h"
#include "IMS_ModuleFunction_Thruster.h"
#include "IMS_ModuleFunction_Rcs.h"
#include "IMS_ModuleFunction_Factory.h"



IMS_ModuleFunction_Factory::IMS_ModuleFunction_Factory()
{

}


IMS_ModuleFunction_Factory::~IMS_ModuleFunction_Factory()
{

}

//creates and returns a new ModuleFunction object. Modify this function if you add new module functionalities!
IMS_ModuleFunction_Base *IMS_ModuleFunction_Factory::CreateNewModuleFunction(IMS_ModuleFunctionData_Base *_data, IMS_Module *module)
{
	switch (_data->GetType())
	{
	case MTYPE_PRESSURISED:
	{
		IMS_ModuleFunctionData_Pressurised *data = (IMS_ModuleFunctionData_Pressurised*)_data;
		return new IMS_ModuleFunction_Pressurised(data, module);
	}
	case MTYPE_ANIM:
	{
		IMS_ModuleFunctionData_Animation *data = (IMS_ModuleFunctionData_Animation*)_data;
		return new IMS_ModuleFunction_Animation(data, module);
	}
	case MTYPE_COMM:
	{
		IMS_ModuleFunctionData_Comm *data = (IMS_ModuleFunctionData_Comm*)_data;
		return new IMS_ModuleFunction_Comm(data, module);
	}
	case MTYPE_TANK:
	{
		IMS_ModuleFunctionData_Tank *data = (IMS_ModuleFunctionData_Tank*)_data;
		return new IMS_ModuleFunction_Tank(data, module);
	}
	case MTYPE_THRUSTER:
	{
		IMS_ModuleFunctionData_Thruster *data = (IMS_ModuleFunctionData_Thruster*)_data;
		return new IMS_ModuleFunction_Thruster(data, module, true);
	}
	case MTYPE_RCS:
	{
		IMS_ModuleFunctionData_Rcs *data = (IMS_ModuleFunctionData_Rcs*)_data;
		return new IMS_ModuleFunction_Rcs(data, module);
	}

	default:
		return NULL;
	}
}


//creates a new ModuleFunctionData object based on the passed function type. Modify this function if you add new module functionalities!
IMS_ModuleFunctionData_Base *IMS_ModuleFunction_Factory::CreateNewModuleFunctionData(FUNCTIONTYPE type)
{
	switch (type)
	{
	case MTYPE_PRESSURISED:
		return new IMS_ModuleFunctionData_Pressurised();
	case MTYPE_ANIM:
		return new IMS_ModuleFunctionData_Animation();
	case MTYPE_COMM:
		return new IMS_ModuleFunctionData_Comm();
	case MTYPE_TANK:
		return new IMS_ModuleFunctionData_Tank();
	case MTYPE_THRUSTER:
		return new IMS_ModuleFunctionData_Thruster();
	case MTYPE_RCS:
		return new IMS_ModuleFunctionData_Rcs();
	default:
		return NULL;
	}

}

//returns a string identifier for a module function type. Modify this function if you add new module functionalities!
string IMS_ModuleFunction_Factory::GetFunctionTypeString(FUNCTIONTYPE type)
{
	string retString("INVALID");

	switch (type)
	{
	case MTYPE_PRESSURISED:
		retString = MTYPE_PRESSURISED_ID;
		break;
	case MTYPE_ANIM:
		retString = MTYPE_ANIM_ID;
		break;
	case MTYPE_COMM:
		retString = MTYPE_COMM_ID;
		break;
	case MTYPE_TANK:
		retString = MTYPE_TANK_ID;
		break;
	case MTYPE_THRUSTER:
		retString = MTYPE_THRUSTER_ID;
		break;
	case MTYPE_RCS:
		retString = MTYPE_RCS_ID;
		break;
	}

	return retString;
}


//returns the module function type from a string identifier. Modify this function if you add new module functionalities!
FUNCTIONTYPE IMS_ModuleFunction_Factory::GetFunctionTypeFromString(string identifier)
{
	//convert the string to all caps
	transform(identifier.begin(), identifier.end(), identifier.begin(), ::toupper);

	if (identifier == MTYPE_PRESSURISED_ID) return MTYPE_PRESSURISED;
	if (identifier == MTYPE_ANIM_ID) return MTYPE_ANIM;
	if (identifier == MTYPE_COMM_ID) return MTYPE_COMM;
	if (identifier == MTYPE_TANK_ID) return MTYPE_TANK;
	if (identifier == MTYPE_THRUSTER_ID) return MTYPE_THRUSTER;
	if (identifier == MTYPE_RCS_ID) return MTYPE_RCS;
	return MTYPE_NONE;
}


