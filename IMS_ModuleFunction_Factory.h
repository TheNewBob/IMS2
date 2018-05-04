#pragma once
/**
 * \file IMS_ModuleFUnction_Factory.h
 * Contains mappings from modulefunction string to enum identifiers.
 */

/**
 * A modulefunction defining a pressurised volume.
 * \see IMS_ModuleFunctionData_Pressurised
 */
static const string MTYPE_PRESSURISED_ID = "PRESSURISED";

/**
 * A modulefunction defining an animation.
 * \see IMS_ModuleFunctionData_Animation
 */
static const string MTYPE_ANIM_ID = "ANIMATION";

/**
 * A modulefunction defining a communications antena
 * \see IMS_ModuleFunctionData_Comm
 */
static const string MTYPE_COMM_ID = "COMMUNICATION";

/**
 * A modulefunction defining a propellant tank.
 * \see IMS_ModuleFunctionData_Tank
 */
static const string MTYPE_TANK_ID = "TANK";

/**
 * A modulefunction defining a thruster.
 * \see IMS_ModuleFunctionData_Thruster
 */
static const string MTYPE_THRUSTER_ID = "THRUSTER";

/**
 * A modulefunction defining a thruster intended for use in the reaction controll system.
 * \see IMS_ModuleFunctionData_Rcs
 */
static const string MTYPE_RCS_ID = "RCS";

/**
 * A modulefunction defining a landing gear
 */
static const string MTYPE_GEAR_ID = "GEAR";

class IMS_ModuleFunction_Factory
{
public:
	IMS_ModuleFunction_Factory();
	~IMS_ModuleFunction_Factory();

	/**
	 * \return A new modulefunction instance.
	 * \param _data A pointer to the data instance defining this modulefunction
	 * \param module A pointer to the module the modulefunction is a part of
	 */
	static IMS_ModuleFunction_Base *CreateNewModuleFunction(IMS_ModuleFunctionData_Base *_data, IMS_Module *module);
	
	/**
	 * \return a new data instance appropriate for the passed function type
	 * \param type The function type identifier for the module function you want to store data for
	 */
	static IMS_ModuleFunctionData_Base *CreateNewModuleFunctionData(FUNCTIONTYPE type);
	
	/**
	 * \return The config file string of a certain modulefunction
	 * \param type The module function identifier you want to know the corresponding string identifier for
	 */
	static string GetFunctionTypeString(FUNCTIONTYPE type);
	
	/**
	 * \return The type identifier of a module function, MTYPE_NONE if the passed string does not correspond to a valid modulefunction config string.
	 * \param identifier The config file string you want to know the corresponding type identifier for
	 */
	static FUNCTIONTYPE GetFunctionTypeFromString(string identifier);

	static Oparse::OpModelDef GetModuleFunctionDataFactories(vector<IMS_ModuleFunctionData_Base*> &receiver);
};

