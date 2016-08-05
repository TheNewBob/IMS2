
#pragma once

using namespace std;

class IMS_ModuleFunctionData_Base;

/**
 * \brief This class contains non-orbiter specific static data about a module that is the same for every module.
 *
 * for every class inheriting from IMS_Module there should also be a class inheriting from this class containing static data of the module type.
 * this class or any class inheriting from it should not use any calls to or typedefs of the Orbiter API! They will be used to integrate IMS data into a third-party editor.
 * The implementation of this class is a work in progress. Currently it only reads a module name and its mass from the config file.
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

