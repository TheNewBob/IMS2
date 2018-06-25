#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Animation.h"
#include "Oparse.h"

using namespace Oparse;


OpModelDef ANIMATIONDATA::GetModelDef()
{
	return OpModelDef() = {
		{ "id", { _Param(id), {_REQUIRED()} } },
		{ "duration", { _Param(duration), {} } },
		{ "type", { _Param(type, true), {_REQUIRED(), _ISANYOF(vector<string>() = { "sequence", "continuous", "track" }) } } },
		{ "facing", { _Param(facing), { _REQUIREDBY("type", "track") } } },
		{ "dependency", { _Block<ANIMATIONDEPENDENCY>(dependencies), {} } },
		{ "comp", { _ModelFactory<ANIMCOMPONENTDATA>(components), { _REQUIRED() } } }
	};
}

OpModelDef ANIMCOMPONENTDATA::GetModelDef()
{
	return OpModelDef() = {
		{ "groups", { _List(groups, " "), { _REQUIRED() } } },
		{ "type", { _Param(type, true), { _REQUIRED(), _ISANYOF(vector<string>() = { "rotate", "translate", "scale" }) } } },
		{ "origin", { _Param(reference), {} } },
		{ "translate", { _Param(reference), { _REQUIREDBY("type", "translate")} } },
		{ "axis", { _Param(axis), { _REQUIREDBY("type", "rotate") } } },
		{ "scale", { _Param(axis), { _REQUIREDBY("type", "scale") } } },
		{ "range", { _Param(range), { _RANGE(-360, 360)} } },
		{ "duration", { _List(duration, " "), { _LENGTH(2, 2) } } },
		{ "parent", { _Param(parent), {} } }
	};
}

OpMixedList * ANIMATIONDEPENDENCY::GetMapping()
{
	return _MixedList(OpValues() = {
			{ _Param(direction), {} },
			{ _Param(dependencyid), {} },
			{ _Param(dependencystate), {} }
		}, " ");
}


OpModelDef IMS_ModuleFunctionData_Animation::GetModelDef()
{
	return OpModelDef() = {
		{ "anim", { _ModelFactory<ANIMATIONDATA>(animations), { _REQUIRED(), _LENGTH(1, INT_MAX) } } }
	};
}



IMS_ModuleFunctionData_Animation::IMS_ModuleFunctionData_Animation()
{
	type = MTYPE_ANIM;
}


IMS_ModuleFunctionData_Animation::~IMS_ModuleFunctionData_Animation()
{
}


