
#pragma once

using namespace std;

class IMS_ModuleFunctionData_Base;


/**
 * \brief This class contains non-orbiter specific static data about a module that is the same for every module.
 *
 * <h3> Config file parameters </h3>
 * These parameters are valid for any IMS module:
 * \code
 *	mass = <float kg>					//the modules dry mass in kg.
 *	[name] = <string name>				//a name for this module type that will be used in variious displays in the UI.
 * \endcode
 */
class IMS_General_ModuleData
{
public:
	IMS_General_ModuleData();
	~IMS_General_ModuleData();

	/**
	 * \brief Loads the data from an Orbiter cfg
	 * \param configfile The name of the configfile
	 * \param cfg The IMSFILE structure wrapping the cfg file in an appropriate format for the current platform
	 * \return True if the data was loaded without errors, false otherwise
	 */
	virtual bool LoadFromFile(string configfile, IMSFILE cfg);

	/**
	 * \return The dry mass of the module, without any additional mass from mModuleFunctions, Components or Cargo
	 */
	double getMass();
	
	/**
	 * \return The name given to this module in the config file
	 */
	string getName();

	/**
	 * \return A reference to a list containing the static data of all contained ModuleFunctions
	 */
	vector<IMS_ModuleFunctionData_Base*> &GetFunctionData() { return functiondata; };

protected:
	double _mass;												//!< Stores the dry mass of this module 
	string _name;												//!< Stores the name of this module
	vector<IMS_ModuleFunctionData_Base*> functiondata;			//!< A list containing static data of all ModuleFunctions in this module

};

