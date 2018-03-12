#pragma once
//#include "IMS_General_ModuleData.h"

/**
 * \brief Holds static data for a pressurised (read: habitable) volume in a module.
 *
 * <h3> Config file structure </h3>
 * Following are valid parameters to define a pressurised volume inside a module:
 * \code
 * BEGIN_MODULEFUNCTION PRESSURISED
 *		Volume = <float m^3>			//pressurised volume in cubic meters
 * END_MODULEFUNCTION
 * \endcode
 */
class IMS_ModuleFunctionData_Pressurised :
	public IMS_ModuleFunctionData_Location
{
public:
	IMS_ModuleFunctionData_Pressurised();
	~IMS_ModuleFunctionData_Pressurised();

};

