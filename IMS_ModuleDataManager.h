//this class manages static module data. If two modules are the same, their static data is the same, so there's no reason to load and store it twice.
#pragma once
#include <map>

class IMS_General_ModuleData;
class IMS_Orbiter_ModuleData;

//struct containing all static moduledata
struct STATICMODULEDATA
{
	IMS_General_ModuleData *moduleData = NULL;
	IMS_Orbiter_ModuleData *orbiterData = NULL;
};


enum CONSUMABLESTATE
{
	GAS,
	LIQUID,
	SOLID
};

class IMS_Component_Model_Base;

/**
* \brief struct used to store static properties of propellant types
*/
struct CONSUMABLEDATA
{
	int id;												//!< generated when reading consumables from file. Serves for faster identification than by string
	std::string name;										//!< the name of the propellant
	std::string shorthand;								//!< The shorthand describing the consumable
	float density;										//!< density in kg/m^3
	CONSUMABLESTATE state;								//!< liquid or solid
	
	/**
	 * \brief use for quick identification by the consumables shorthand
	 */
	bool operator==(string shorthnd)
	{
		return shorthand == shorthnd;
	}
};

class IMS_ModuleDataManager
{
public:
	IMS_ModuleDataManager();
	~IMS_ModuleDataManager();

	static STATICMODULEDATA GetStaticModuleData(string configfile, IMSFILE cfg);

	/**
	 * \return A pointer to the PROPELLANTDATA whose name matches the passed string
	 * \param propellanttype A string containing the shorthand of the propellant type you want to retrieve
	 */
	static CONSUMABLEDATA *GetConsumableData(string consumable_short);

	/**
	 * \return The id of a given consumabletype, or -1 if the type was not found
	 * \param propellanttype A string containing the shorthand of the propellant type you want the id of
	 */
	static int GetConsumableId(string consumable_short);

	/**
	* \return A pointer to the PROPELLANTDATA whose id matches the passed int
	* \param id The id of the propellant type you want to retrieve
	*/
	static CONSUMABLEDATA *GetConsumableData(int id);

	/**
	 * \return An iterator over available consumables
	 */
	static vector<CONSUMABLEDATA> &GetConsumablesList() { return consumabledata; };

	/**
	* \return A list with consumables compatible with a tank that stores a reference consumable
	* \param reference Id of the consumable a storable typically contains
	*/
	static vector<CONSUMABLEDATA*> GetCompatibleConsumables(int reference_id);

	/**
	 * \return The component model for a component of the passed type
	 */
	static IMS_Component_Model_Base *GetComponentModel(string type);



private:
	/**
	 * \brief Loads the propellantdata from config/IMS2/propellanttypes.cfg
	 */
	static void loadConsumableData();

	/**
	* \brief Loads all component files in Config/IMS2/components and subfolders.
	*/
	static void loadComponentData(string path);

	static vector<CONSUMABLEDATA> consumabledata;				//!< stores all propellant data objects
	static map<string, STATICMODULEDATA> _staticModuleData;
	static map<string, IMS_Component_Model_Base*> components;	//!< stores all component models
};

