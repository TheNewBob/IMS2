
#pragma once

using namespace std;

class IMS_ModuleFunctionData_Base;
class SimpleShape;

/**
 * \brief Contains data to construct the hullshape as loaded from file.
 * \note This is only used to defer shape construction until the entire file has been loaded.
 */
struct HULLSHAPEDATA
{
	string shape = "";
	VECTOR3 shapeparams;
	VECTOR3 scale = _V(1, 1, 1);
	VECTOR3 pos = _V(0, 0, 0);
	VECTOR3 dir = _V(0, 0, 1);
	VECTOR3 rot = _V(0, 1, 0);
};

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
	 * \return A pointer to the hull shape of the module
	 */
	SimpleShape *GetHullShape() { return hullshape; }
	
	/**
	 * \return A reference to a list containing the static data of all contained ModuleFunctions
	 */
	vector<IMS_ModuleFunctionData_Base*> &GetFunctionData() { return functiondata; };

protected:
	double _mass;												//!< Stores the dry mass of this module 
	string _name;												//!< Stores the name of this module
	vector<IMS_ModuleFunctionData_Base*> functiondata;			//!< A list containing static data of all ModuleFunctions in this module

	SimpleShape *hullshape = NULL;								//!< The vertices of the hull shape of the module, module relative.
	
private:
	/**
	 * \brief creates the hull shape based on the parameters read from the scenario line
	 * \param shapedata The data defining the nature of the shape.
	 */
	void createHullShape(HULLSHAPEDATA shapedata);
	
	/**
	 * \brief Reads the next line of a SHAPE block from the config file
	 * \param file The file to read from. It is implied that the last line read from the file was BEGIN_SHAPE.
	 * \return The data describing the shape read from the file.
	 */
	HULLSHAPEDATA readShape(IMSFILE file);
};

