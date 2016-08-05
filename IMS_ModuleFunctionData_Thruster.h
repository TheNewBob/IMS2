#pragma once

/**
 * \brief Stores the static data of a basic thruster.
 *
 * Config parameters:
 * dir = [float, float, float]					;The direction of thrust, relative to the module. Direction of thrust means "the direction the thruster is pushing towards", not "the direction stuff is flying out of the thruster"!
 * pos = [float, float, float] (optional)		;The position of the point the thrust is originating at, relative to the module. Default is 0, 0, 0, which will be perfectly fine for most applications.
 *
 * BEGIN_EXHAUST									;Begins an exhaust definition block. Thrusters can have an arbitrary number of exhausts, but must have at least one!
 *	valid parameters in an exhaust block:
 *	length = [float]							;The length of the exhaust plume at maximum thrust, in meters.
 *	width = [float]								;The width of the exhaust plume at maximum thrust, in meters.
 *	pos = [float, float, float]					;The origin of the exhaust plume, relative to the module.
 *	dir = [float, float, float] (optional)		;The direction of the exhaust plume. Default is the inverted direction of thrust of the thruster.
 * END_EXHAUST									;Ends an exhaust definition block.
 *
 * BEGIN_THRUSTERMODE [string thrustermode_name]	;Begins a new thrustermode definition block. A thruster can have an arbitrary amount of thrustermodes, but must have at least one!
 *	valid parameters in a thrustermode block:
 *	exhaustlength = [float] (optional)			;A scaling factor for the length of the exhaust plume at maximum thrust, between 0 and 1. Default is 1.
 *	exhaustwidth = [float] (optional)			;A scaling factor for the width of the exhaust plume at maximum thrust, between 0 and 1. Default is 1.
 *	consumable = [string float]					;The shorthand of a consumable (see Consumables.cfg for valid values) the thruster uses as propellant, followed by its weight in the mixture.
 *												;An arbitrary number of consumables can be declared per thrustermode. The weight for the mixture is in "parts", e.g. a consumable with a weight of 1 and a consumable with the weight of 2.5 results in a ratio of 1:2.5.
 *	thrust = [float]							;The maximum thrust of the thruster, in Newton.
 *	isp = [float]								;The exhaust velocity of the thruster, in m/s.
 * END_THRUSTERMODE								;Ends a thrustermode definition block.
 *
 * Config example:
 *
 * BEGIN_MODULEFUNCTION THRUSTER
 *	name = F1
 *	dir = 0 0 1
 *
 *	BEGIN_EXHAUST
 *		length = 20
 *		width = 1
 *		pos = 0 0 -1
 *	END_EXHAUST
 *
 *	BEGIN_THRUSTERMODE default
 *		Consumable = LOX 2.56
 *		Consumable = RP1 1
 *		Thrust = 7740500
 *		Isp = 2982
 *	END_THRUSTERMODE
 *
 * END_MODULEFUNCTION
 */
class IMS_ModuleFunctionData_Thruster :
	public IMS_ModuleFunctionData_Base
{
public:
	IMS_ModuleFunctionData_Thruster();
	~IMS_ModuleFunctionData_Thruster();

	/**
	 * \return A pointer to the thrustermode with the requested index 
	 * \param mode The index of the requested thrustermode in the thrustermode list
	 */
	THRUSTERMODE *GetThrusterMode(int mode);

	/**
	 * \return The number of modes supported by this thruster
	 */
	int getNumberOfModes() { return thrustermodes.size(); };
	
	THRUSTEREXHAUST *GetThrusterExhaust(int idx);

	int GetNumberOfExhausts() { return exhausts.size(); };
	/**
	 * \return The direction of thrust of the thruster, module relative
	 */
	VECTOR3 GetThrustDirection() { return thrustdirection; };

	/**
	 * \return The position of the thruster, module relative.
	 */
	VECTOR3 GetThrusterPosition() { return thrusterpos; };

	virtual bool HasGui() { return true; };

protected:
	virtual bool processConfigLine(vector<string> &tokens);
	virtual bool validateData(string configfile);

	vector<THRUSTERMODE> thrustermodes;
	vector<THRUSTEREXHAUST> exhausts;
	VECTOR3 thrustdirection = _V(0, 0, 0);
	VECTOR3 thrusterpos = _V(0, 0, 0);

private:
	/**
	* \brief processes a senario line for a thrustermode
	* \param tokens The scenario line split into single items
	*/
	void processMode(vector<string> &tokens);

	/**
	* \brief processes a senario line for an exhaust definition
	* \param tokens The scenario line split into single items
	*/
	void processExhaust(vector<string> &tokens);

	/**
	 * \brief validates the thruster modes read from the config file
	 * \throws invalid_argument if there is a problem with thruster modes
	 */
	void validateModes();

	/**
	* \brief validates the exhausts read from the config file
	* \throws invalid_argument if there is a problem with exhausts
	*/
	void validateExhausts();
};