#pragma once

/**
 * \brief Abstract base class for storing generic static data of module functions loaded from a config file
 *
 * Every derivative of IMS_ModuleFunction_Base() needs a corresponding derivative of this class
 * to load and store its configfile data.
 *
 * Module functions are specific abilities that can be added to a module in a config file, like thrusters,
 * tanks, habitat etc. Many modules will have only one module function, but an arbitrary number of 
 * modulefunctions can be defined for any module.
 *
 * <h3> Config structure: </h3>
 * An arbitrary number of module functions can be defined for any module.
 * Any module function must be enclosed in a module function block like this:
 * \code
 * BEGIN_MODULEFUNCTION <modulefunction-ID>
 *		<modulefunction specific parameter>
 *		 .
 *		 .	
 * END_MODULEFUNCTION
 * \endcode
 * For a list of valid modulefunction ids, see the documentation for IMS_ModuleFunctionFactory.h
 *
 * <h4> Generic modulefunction parameters </h4>
 * These parameters can be added to any module function
 * \code
 * name = <string name>				//a name for this module function type that will be used in various displays in the UI
 * \endcode
 * \see IMS_ModuleFunctionFactory.h
 */
class IMS_ModuleFunctionData_Base
{
public:
	IMS_ModuleFunctionData_Base() { type = MTYPE_NONE; };
	virtual ~IMS_ModuleFunctionData_Base() {};

	virtual Oparse::OpModelDef GetModelDef() = 0;
	/**
	 * \brief This function serves to check whether a module function has a GUI before creating it.
	 *
	 * \note If your ModuleFunction has its own GUI, overload this function and return true, as simple as that.
	 * \return Always false
	 */
	virtual bool HasGui() { return false; };

	/**
	 * \return The type of the ModuleFunction this data is for
	 */
	FUNCTIONTYPE GetType() { return type; };
	
	/**
	 * \return The name of this ModuleFunction
	 */
	string GetName() { return name; };

	/**
	 * \brief Override if you need to do processing once the parsing is done.
	 */
	virtual void PostParse() {};

protected:
	FUNCTIONTYPE type;								//!< Stores the type of the ModuleFunction
	string name = "";								//!< Stores the Name of the ModuleFunction
};

