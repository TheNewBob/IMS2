#pragma once


struct CONSUMABLEDATA;

/**
 * \brief Stores the static data of a tank.
 *
 * Config parameters:
 * contains = [string]				;the name of an item defined in Config/IMS2/gojuice.cfg
 * volume = [float]					;the usable tank volume in m^3
 * convertible = true/false			;optional. Whether thetank can be converted to habitation space. Default is true, has no effect when containing solid fuel (always false).
 *
 * Config example:
 *
 *	BEGIN_MODULEFUNC TANK
 *		contains = LH2
 *		volume = 5
 *		convertible = false
 *	END_MODULEFUNC
 */
class IMS_ModuleFunctionData_Tank :
	public IMS_ModuleFunctionData_Pressurised
{
public:
	IMS_ModuleFunctionData_Tank();
	~IMS_ModuleFunctionData_Tank();

	virtual bool HasGui() { return true; };

	/**
	 * \return A pointer to the consumable type this tank contains
	 */
	CONSUMABLEDATA *getPropellant() { return propellant; };
	
protected:
	bool processConfigLine(vector<string> &tokens);
	
	bool validateData(string configfile);

	CONSUMABLEDATA *propellant = NULL;							//!< pointer to the information of the contents of this tank
	bool convertible = true;								//!< whether the tank can be converted to living space
};

