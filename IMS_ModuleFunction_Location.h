#pragma once

class IMS_ComponentModel_Base;
class IMS_Component_Base;
class IMS_ModuleFunctionData_Location;

/**
 * \brief Abstract base class for module functions that are also a location.
 */
class IMS_ModuleFunction_Location :
	public IMS_ModuleFunction_Base, public IMS_Location
{
public:
	IMS_ModuleFunction_Location(IMS_ModuleFunctionData_Location *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> contexts, double maxVolume);
	virtual ~IMS_ModuleFunction_Location();

	/**
	 * \brief Adds a fixed component to this modulefunction.
	 * Components can be disassembled again, but cannot be easily moved between locations.
	 */
	void CreateComponent(string componentName);
	
	/**
	 * \brief Removes a fixed component from this module
	 */
	void RemoveComponent(IMS_Component_Base *component);

	/**
	 * \return The maximum volume of this modulefunction, in m3.
	 */
	double GetMaxVolume() { return maxVolume; };

	/**
	 * \return The currently available volume of this modulefunction, in m3.
	 */
	double GetAvailableVolume() { return availableVolume; };

	/**
	 * \return The (wet) mass of components installed in this module, in kg.
	 * \note The value obtained by this should only be used for informative or modulefunction internal purposes.
	 *	The total mass of the module function should always be retrieved using GetMass().		
	 */
	double GetComponentMass() { return componentMass; };

	IMS_ModuleFunctionData_Location *GetData();

	void IMS_ModuleFunction_Location::PreStep(double simdt, IMS2 *vessel);

protected:
	vector<IMS_Component_Base*> components;

private:
	double maxVolume = -1;
	double availableVolume = -1;
	double componentMass = 0;
	
	/**
	 * \brief Calculates volume left over and total mass of all components.
	 */
	void calculateComponentProperties();
	IMS_ModuleFunctionData_Location *data;

};

