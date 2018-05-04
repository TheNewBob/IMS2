#pragma once


struct CONSUMABLEDATA;

/**
 * \brief Stores the static data of a tank.
 *
 * <h3> Config parameters </h3>
 * \code
 * contains = <string id>			//the id of an item defined in Config/IMS2/consumables.cfg
 * volume = <float m^3>				//the usable tank volume in m^3
 * [convertible] = <true/false>		//optional. Whether the tank can be converted to habitation space. Default is true, has no effect when containing solid fuel (always false).
 * \endcode
 * <h4> Config example </h4>
 * \code
 *	BEGIN_MODULEFUNCTION TANK
 *		contains = LH2
 *		volume = 5
 *		convertible = false
 *	END_MODULEFUNC
 * \endcode
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

	Oparse::OpModelDef GetModelDef();
	
protected:
	bool processConfigLine(vector<string> &tokens);
	
	bool validateData(string configfile);

	CONSUMABLEDATA *propellant = NULL;							//!< pointer to the information of the contents of this tank
	bool convertible = true;								//!< whether the tank can be converted to living space
};

