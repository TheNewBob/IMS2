#pragma once
class IMS_Component_Base : public EventHandler
{
public:
	IMS_Component_Base(IMS_Component_Model_Base *data, IMS_Module *module);
	virtual ~IMS_Component_Base();

	/**
	* \brief called on SimStart, after LoadState() but before AddComponentToVessel()
	* @see LoadState()
	* @see processScenarioLine()
	* @see AddFunctionToVessel()
	*/
	virtual void PostLoad() {};

	/**
	* \brief overload to serialise component state.
	* @param scn the FILEHANDLE to the opened scenario file
	* \note The BEGIN_COMPONENT and END_COMPONENT tags are
	*	written by the containing module function, so you really just have to write the data.
	*/
	virtual string Serialize() = 0;

	/**
	* \brief Restores component state from serialised string.
	* The serialized string must not contain whitespace!
	* @param scn the FILEHANDLE to the opened scenario file
	* \note Overload processScenarioLine() to implement loading from scenario!
	* @see processScenarioLine()
	* \returns
	*/
	virtual void Deserialize(string data) = 0;

	/**
	* \brief Everything that needs to be done when the component is removed from a vessel should be implemented in an overload of this function.
	* @param vessel The vessel the module is to be added to
	* \note This function gets called every time the module is added to a new vessel, which happens on startup
	*	(whether loaded from scenario or not), at integration and at splitting.
	*/
//	virtual void AddFunctionToVessel(IMS2 *vessel) = 0;

	/**
	* \brief Everything that needs to be done when the module is removed from a vessel should be implemented in an overload of this function.
	* @param vessel The vessel the module is being removed from
	* \note This function only gets called on splitting, one frame before AddFunctionToVessel() is called to add it to the new vessel
	*/
//	virtual void RemoveFunctionFromVessel(IMS2 *vessel) = 0;

	/**
	* \brief Returns the current component mass
	*/
	virtual double GetMass() = 0;

	/**
	 * \return The volume the component takes up, in cubic meters.
	 */
	double GetVolume();

	/**
	* \brief gets called at every clbkPreStep of the containing vessel.
	* @param simdt elapsed time since the last frame
	* @param vessel the vessel this module function belongs to
	*/
	virtual void PreStep(double simdt, IMS2 *vessel) = 0;

	/**
	* \brief is called at the end of every clbkPreStep on the vessel.
	* This essentially serves the purpose to process the loopback pipe
	* of the eventhandler. It is executed after everybody has done its thing
	* and sent all its messages, and is used to finalise the vessel state
	* before Orbiter applies the current timestep. It should NOT be overriden by inheriting classes.
	* If you push events to the waiting queue, this is the time they will fire,
	* and you get to finalise your states by reacting to them.
	*/
	void PreStateUpdate();

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
	* \brief receives all scenario lines concerning this component, one after the other.
	* @param line A string object containing the line read from the scenario
	* use this function to implement state initialisation when your component is loaded from scenario
	*/
	virtual bool processScenarioLine(string line) = 0;
};

