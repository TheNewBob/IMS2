#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "IMS_ModuleDataManager.h"
#include "Oparse.h"

using namespace Oparse;

// TODO replaced 350 lines of parsing code with 45 lines of mapping

OpModelDef THRUSTERMODE::GetModelDef() 
{
	// special parsing for consumable id
	auto consumableDef = _MixedList(OpValues() = {
		{ _Param([&](string value) {
			int propellantid = IMS_ModuleDataManager::GetConsumableId(value);
			if (propellantid == -1) return "consumable not found in consumables.cfg: " + value;
			else
			{
				Propellants.push_back(propellantid);
				return string("");
			}
		}), {} },
		{ _List(Ratio), {} }
	}, " \t");
	
	return OpModelDef() = {
		{ "name", { _Param(Name), {} } },
		{ "thrust", { _Param(Thrust), { _REQUIRED(), _MIN(0.0) } } },
		{ "isp", { _Param(Isp), { _REQUIRED(), _MIN(0.0) } } },
		{ "efficiency", { _Param(Efficiency), { _RANGE(0.1e-9, 1.0)} } },
		{ "exhaustlength", { _Param(Exhaustlength), { _RANGE(0.0, 1.0) } } },
		{ "exhaustwidth", { _Param(Exhaustwidth), { _RANGE(0.0, 1.0) } } },
		{ "consumable", { consumableDef, { _REQUIRED() } } }
	};
}

OpModelDef THRUSTEREXHAUST::GetModelDef()
{
	return OpModelDef() = {
		{ "length", { _Param(length), { _REQUIRED() } } },
		{ "width", { _Param(width), { _REQUIRED() } } },
		{ "pos", { _Param(pos), {} } },
		{ "dir", { _Param(dir), {} } }
	};
}

OpModelDef IMS_ModuleFunctionData_Thruster::GetModelDef()
{
	return OpModelDef() = {
		{ "name", { _Param(name), { _REQUIRED() } } },
		{ "exhaust", { _ModelFactory<THRUSTEREXHAUST>(exhausts), { _REQUIRED() } } },
		{ "thrustermode", { _ModelFactory<THRUSTERMODE>(thrustermodes), { _REQUIRED() } } },
		{ "dir", { _Param(thrustdirection), {} } },
		{ "pos", { _Param(thrusterpos), {} } }
	};
}


IMS_ModuleFunctionData_Thruster::IMS_ModuleFunctionData_Thruster()
{
	type = MTYPE_THRUSTER;
}


IMS_ModuleFunctionData_Thruster::~IMS_ModuleFunctionData_Thruster()
{
	for (UINT i = 0; i < exhausts.size(); ++i)
	{
		delete exhausts[i];
	}

	for (UINT i = 0; i < thrustermodes.size(); ++i)
	{
		delete thrustermodes[i];
	}
}

THRUSTERMODE *IMS_ModuleFunctionData_Thruster::GetThrusterMode(int mode)
{
	Olog::assertThat([this, mode]() { return (UINT)mode < thrustermodes.size(); }, "Trying to retrieve non-existing thrustermode!");
	return thrustermodes[mode];
}

THRUSTEREXHAUST *IMS_ModuleFunctionData_Thruster::GetThrusterExhaust(int idx)
{
	Olog::assertThat([this, idx]() { return (UINT)idx < exhausts.size(); }, "Trying to retrieve non-existing exhaust!");
	return exhausts[idx];
}


void IMS_ModuleFunctionData_Thruster::PostParse()
{
	for (UINT i = 0; i < exhausts.size(); ++i)
	{
		if (exhausts[i]->dir.x == 0 &&
			exhausts[i]->dir.y == 0 &&
			exhausts[i]->dir.z == 0)
		{
			//no exhaust dir defined, assume opposite of thrust direction
			exhausts[i]->dir = thrustdirection * -1;
		}
	}
}
