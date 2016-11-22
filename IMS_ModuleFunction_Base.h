#pragma once

class IMS_ModuleFunctionData_Base;
class GUI_ModuleFunction_Base;

/**
 * \brief abstract class that defines the API for module functionalities.
 *
 * A module function is an object that does things for a module which not every module
 * must be capable of doing. In General, these things should have some relation
 * to the Orbiter API. If this is not the case, the functionality should be 
 * implemented as a component instead (Compnent Interface not yet implemented,
 * we'll get there...)
 * \note Apart from defining the interface between its implementations
 *	and IMS_Module, it also handles registration of the EventHandler
 *	for the module function and abstracts loading states from scenario.
 */
class IMS_ModuleFunction_Base : public EventHandler
{
public:
	/** 
	 * \brief Constructor.
	 * @param _data the IMS_ModuleFunctionData object containing the static config data for this module function
	 * @param _module The IMS_Module object containing this module function
	 * @param _type An identifer for the type of this module function
	 */
	IMS_ModuleFunction_Base(IMS_ModuleFunctionData_Base *_data, IMS_Module *_module, FUNCTIONTYPE _type);
	virtual ~IMS_ModuleFunction_Base();

	/**
	 * \brief called on SimStart, after LoadState() but before AddFunctionToVessel()
	 * @see LoadState()
	 * @see processScenarioLine()
	 * @see AddFunctionToVessel()
	 */
	virtual void PostLoad()=0;
	
	/**
	 * \brief overload to save modulefunction specific stuff to scenario.
	 * @param scn the FILEHANDLE to the opened scenario file
	 * \note The BEGIN_MODULEFUNC and END_MODULEFUNC tags are
	 *	written by the module, so you really just have to write the data.
	 */
	virtual void SaveState(FILEHANDLE scn) = 0;
	
	/**
	 * \brief Sends every scenario line concerning this modulefunction function to processScenarioLine().
	 * @param scn the FILEHANDLE to the opened scenario file
	 * \note Overload processScenarioLine() to implement loading from scenario!
	 * @see processScenarioLine()
	 * \returns
	 */
	void LoadState(FILEHANDLE scn);
	
	/**
	* \brief Everything that needs to be done when the modulefunction is removed from a vessel should be implemented in an overload of this function.
	* @param vessel The vessel the module is to be added to
	* \note This function gets called every time the module is added to a new vessel, which happens on startup
	*	(whether loaded from scenario or not), at integration and at splitting.
	*/
	virtual void AddFunctionToVessel(IMS2 *vessel) = 0;
	
	/**
	* \brief Everything that needs to be done when the module is removed from a vessel should be implemented in an overload of this function.
	* @param vessel The vessel the module is being removed from
	* \note This function only gets called on splitting, one frame before AddFunctionToVessel() is called to add it to the new vessel
	*/
	virtual void RemoveFunctionFromVessel(IMS2 *vessel) = 0;
	
	/**
	 * \brief Returns the current modulefunctions mass
	 * \note A ModuleFunction should only have a mass if that mass is not static.
	 * \note So if you have for example a propellant tank, the propellant mass should
	 *	be part of the ModuleFunction, while the mass of the tankage should not be.
	 */
	virtual double GetMass() { return 0; };

	/**
	 * \brief gets called at every clbkPreStep of the containing vessel.
	 * @param simdt elapsed time since the last frame
	 * @param vessel the vessel this module function belongs to
	 */
	virtual void PreStep(double simdt, IMS2 *vessel);
	
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
	virtual GUI_ModuleFunction_Base *GetGui() { return NULL; };

	/**
	 * \return The module containing this ModuleFunction
	 */
	IMS_Module *GetModule() { return module; };

protected:
	
	FUNCTIONTYPE type;						//!< An identifier for the type of this module function
	IMS_Module *module = NULL;						//!< Pointer to the IMS_Module containing this module function
	
	/**
	 * \brief receives all scenario lines concerning this modulefunction, one after the other.
	 * @param line A string object containing the line read from the scenario
	 * use this function to implement state initialisation when your module function is loaded from scenario
	 */
	virtual bool processScenarioLine(string line) = 0;

private:
	IMS_ModuleFunctionData_Base *data;		//!< Pointer to the config file data for this ModuleFunction
};

