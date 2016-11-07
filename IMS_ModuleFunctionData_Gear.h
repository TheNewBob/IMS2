#pragma once

/**
* \brief Holds static data to define a piece of landing gear.
*
* <h3> Config parameters </h3>
* \code
* BEGIN_MODULEFUNCTION GEAR
*	TdPos = <float x> <float y> <float z>	//the position of the DEPLOYED touchdown point, module relative.
*	TdDir = <float x> <float y> <float z>	//the DOWN direction of the touchdown point, module relative, as a normalised vector.
*	[Deploy-anim] = <string id>				//the id of the deployment animation (should be a sequential animation). Optional if is undeployable, in which case it will be considered permanently deployed.
*	[TdStiffness] = <float>					//Suspension stiffness (i.e. how hard it is to compress the suspension). Default is 1e6.
*	[TdDamping] = <float>					//Damping coefficient of the landing gear (how much energy is lost when compressing the suspension). Default is 1e5.
* END_MODULEFUNCTION
*\endcode
* A note on tdstiffness and tddamping: In general, it is recommendable to leave these alone and let the engineer of the vessel 
* figure out the optimum number of gears to put on his vessel. The values are still here however if somebody wants to make different
* "weight-classes" of landing gear (for example, gear that is only useful on very massive or very light vessels).
* The deployment animation has to be defined as a separate IMS_ModuleFunction_Animation in the same cconfig.
*
* <h4>config example</h4>
* \code
* BEGIN_MODULEFUNCTION GEAR
*	deploy_anim = deploy
*	tdpos = 0 0 -3.4
*	tddir = 0 0 -1
* END_MODULEFUNCTION
* \endcode
* \see IMS_ModuleFunction_Animation
*/
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
	double tdstiffness = 1e6;
	double tddamping = 1e5;
	VECTOR3 tddir = _V(0, 0, -1);

	bool processConfigLine(vector<string> &tokens);
	bool validateData(string configfile);
	bool candeploy = false;
};

