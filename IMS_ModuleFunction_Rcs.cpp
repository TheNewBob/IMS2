#include "GUIIncludes.h"
#include "ModuleFunctionIncludes.h"
#include "IMS.h"
#include "IMS_Manager_Base.h"
#include "IMS_PropulsionManager.h"
#include "IMS_RcsManager.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "IMS_ModuleFunctionData_Rcs.h"
#include "IMS_ModuleFunction_Thruster.h"
#include "IMS_ModuleFunction_Rcs.h"


IMS_ModuleFunction_Rcs::IMS_ModuleFunction_Rcs(IMS_ModuleFunctionData_Rcs *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Thruster(_data, _module, false), data(_data)
{
	type = MTYPE_RCS;
}


IMS_ModuleFunction_Rcs::~IMS_ModuleFunction_Rcs()
{
}


void IMS_ModuleFunction_Rcs::AddFunctionToVessel(IMS2 *vessel)
{
	propmanager = vessel->GetPropulsionManager();

	//tranform the thrust direction to vessel relative coordinates
	VECTOR3 thrustdirection = mul(module->GetRotationMatrix(), data->GetThrustDirection());
	normalise(thrustdirection);
	//create the thruster on the vessel and store the handle
	thruster = propmanager->AddThruster(data->GetThrusterMode(0), module->GetLocalCoordinates(), thrustdirection);
	//if the thruster is mirrored, create its counterpart, then add both to the rcs manager
	if (data->IsMirrored())
	{
		thrustdirection *= -1;
		mirroredthruster = propmanager->AddThruster(data->GetThrusterMode(0), module->GetLocalCoordinates(), thrustdirection);
		vessel->GetRcsManager()->AddThrusterPair(thruster, mirroredthruster);
	}
	else
	{
		//if it's not mirrored, just add the one thruster to the rcs
		vessel->GetRcsManager()->AddThruster(thruster);
	}

	AddExhaustsToVessel();
}


void IMS_ModuleFunction_Rcs::RemoveFunctionFromVessel(IMS2 *vessel)
{


	//calculate the flowrate of the thruster so the propellant manager can readjust the size of the virtual resource
	THRUSTERMODE *curmode = data->GetThrusterMode(currentthrustermode);
	double maxmassflow = curmode->Thrust / curmode->Isp;

	//remove the thruster from the rcs manager 
	vessel->GetRcsManager()->RemoveThruster(thruster);
	//remove thruster and exhausts completely from the vessel
	propmanager->RemoveExhausts(thruster);
	propmanager->RemoveThruster(thruster, maxmassflow);

	//if the thruster has a mirrored twin, do the same for that
	if (data->IsMirrored())
	{
		vessel->GetRcsManager()->RemoveThruster(mirroredthruster);
		//remove the twin and its exhausts
		propmanager->RemoveExhausts(mirroredthruster);
		propmanager->RemoveThruster(mirroredthruster, maxmassflow);
	}
}


void IMS_ModuleFunction_Rcs::AddExhaustsToVessel()
{
	//create vessel-relative exhausts
	vector<THRUSTEREXHAUST> allexhausts = getRelativeExhausts();
	
	if (data->IsMirrored())
	{
		//if the thruster is mirrored, half of the exhausts will be for one thruster, half for the other
		std::size_t const half_size = allexhausts.size() / 2;
		std::vector<THRUSTEREXHAUST> first_half(allexhausts.begin(), allexhausts.begin() + half_size);
		std::vector<THRUSTEREXHAUST> second_half(allexhausts.begin() + half_size, allexhausts.end());

		propmanager->AddExhausts(thruster, first_half);
		propmanager->AddExhausts(mirroredthruster, second_half);
	}
	else
	{
		//if not mirrored, all exhausts are linked to the one thruster
		propmanager->AddExhausts(thruster, allexhausts);
	}
}


void IMS_ModuleFunction_Rcs::RemoveExhaustsFromVessel()
{
	propmanager->RemoveExhausts(thruster);
	if (data->IsMirrored())
	{
		propmanager->RemoveExhausts(mirroredthruster);
	}
}

