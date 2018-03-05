#pragma once

class IMS_ComponentModel_Base;
class IMS_Component_Base;

/**
 * \brief Abstract base class for module functions that are also a location.
 */
class IMS_ModuleFunction_Location :
	public IMS_ModuleFunction_Base, public IMS_Location
{
public:
	IMS_ModuleFunction_Location(IMS_ModuleFunctionData_Base *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> contexts, double maxVolume);
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


protected:
	vector<IMS_Component_Base*> components;

private:
	double maxVolume = -1;
	double availableVolume = -1;
	double calculateAvailableVolume();

};

