#pragma once

class IMS_Component_Model_Base;

class IMS_Component_Base : public IMS_Movable
{
public:
	IMS_Component_Base(IMS_Component_Model_Base *data, IMS_Location *location);
	virtual ~IMS_Component_Base();

	/**
	* \brief called on SimStart, after LoadState() but before AddComponentToVessel()
	* @see LoadState()
	* @see processScenarioLine()
	* @see AddFunctionToVessel()
	*/
	virtual void PostLoad() {};

	/**
	* \return A string serialization of the dynamic data of the component.
	*/
	string Serialize();

	/**
	* \brief Restores component state from serialized string.
	* \param data A string in the following format: <key>:<value>;<key>:<value>;...
	*/
	void Deserialize(string data);

	/**
	* \brief Returns the current component mass
	*/
	virtual double GetMass() = 0;

	/**
	 * \return The volume the component takes up, in cubic meters.
	 */
	double GetVolume();

	/**
	 * \return The name of this component
	 */
	string GetName();

	/**
	* \brief returns the GUI of this module function, or NULL if this module function doesn't have a user interface.
	* \note Overload this function to return the specific GUI
	* of your module function, otherwise the core will think
	* that the module function has no user input.
	*/
	//virtual GUI_ModuleFunction_Base *GetGui() { return NULL; };

protected:
	IMS_Component_Model_Base *data;

	/**
	 * \brief override to return the keys and values of your components dynamic data for storing in scenario files.
	 */
	virtual void getDynamicData(map<string, string> &keysAndValues) = 0;

	/**
	 * \brief override to initialise dynamic data from scenarios.
	 * \param keysAndValues The keys and associated values that were loaded from the scenario.
	 */
	virtual void setDynamicData(map<string, string> &keysAndValues) = 0;

};

