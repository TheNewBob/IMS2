#pragma once

/**
 * \brief Stores the static data of an Rcs thruster.
 *
 * Config parameters:
 *
 * In general, defining an RCS thruster is no different from defining a normal thruster.
 * All parameters used for a thruster are valid, with the limitation that Rcs thrusters will only ever use
 * the first thrustermode defined. There is one additional parameter to common thrusterdefinitions:
 *
 * mirror = [0 or 1] (optional)			;If set to 1, the thruster will be mirrored, i.e. an exact, inverted duplicate (relative to the module) will be created, exhausts and all.
 *										;You should use this whenever you have a pair of Rcs thrusters that point in opposite directions. Not only is it less to write,
 *										;it also greatly improves the performance of the calculation of firing solutions for your vessels rcs.
 *										;default is 0.
 *
 * Config example:
 *
 * BEGIN_MODULEFUNCTION RCS
 *	dir = 0 1 0
 *	mirror = 1
 *
 *	BEGIN_EXHAUST
 *		length = 0.5
 *		width = 0.1
 *		pos = 0 -0.5 0
 *	END_EXHAUST
 *
 *	BEGIN_THRUSTERMODE rcs
 *		Consumable = N2H4 1
 *		Thrust = 1000
 *		Isp = 2160
 *	END_THRUSTERMODE
 *
 * END_MODULEFUNCTION
 */
class IMS_ModuleFunctionData_Rcs :
	public IMS_ModuleFunctionData_Thruster
{
public:
	IMS_ModuleFunctionData_Rcs();
	~IMS_ModuleFunctionData_Rcs();

	virtual bool HasGui() { return false; };
	bool IsMirrored() { return mirrored; };

protected:
	virtual bool processConfigLine(vector<string> &tokens);
	virtual bool validateData(string configfile);

	bool mirrored = false;				//!< if true, a mirrored thruster will be placed on the same module

	/**
	 * \brief Creates exhausts mirroring the existing ones
	 */
	void mirrorExhausts();
};

