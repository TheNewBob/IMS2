#pragma once

/**
 * \brief Holds static data to define a communications antena, satelite dish and similar.
 *
 * <h3> Config parameters </h3>
 * \code
 * BEGIN_MODULEFUNCTION COMMUNICATION
 *	[Deploy-anim] = <string id>			//the id of the deployment animation. Optional if the antenna doesn't have a deployment animation.
 *	[Search_anim] = <string id>			//the id of the scanning animation (continuously rotating dish or similar). Optional if the antenna doesn't have a scanning animation.
 *	[Tracking_anim] = <string id>       //the id of the tracking animation used to align the antenna with a target. Optional if the antenna doesn't have such an animation.
 * END_MODULEFUNCTION
 *\endcode
 * Note that the most difficult part of a config file for such a module will be defining the actual animations, but this is covered in the documentation for IMS_ModuleFunctionData_Animation
 * \see IMS_ModuleFunction_Animation
 */
class IMS_ModuleFunctionData_Comm :
	public IMS_ModuleFunctionData_Base
{
public:
	friend class IMS_ModuleFunction_Comm;

	IMS_ModuleFunctionData_Comm();
	~IMS_ModuleFunctionData_Comm();

	bool HasGui() { return true; };

private:
	string trackinganimname = "";
	string deployanimname = "";
	string searchanimname = "";

	bool processConfigLine(vector<string> &tokens);
	bool validateData(string configfile);

};

