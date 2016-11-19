#pragma once
#include "EventHandler.h"
class IMS_Manager_Base :
	public EventHandler
{
public:
	IMS_Manager_Base(IMS2 *vessel);
	~IMS_Manager_Base();

	/**
	 * \brief Override to initialise a manager on simulation load if necessary.
	 * Called at the end of clbkLoadState, when all modules have been added to the vessel.
	 * If a manager needs to initialise itself in order to be ready on PostCreation, this is where you do it.
	 */
	virtual void PostLoad() {};

	/**
	 * \brief is called on every clbkPreStep call on the vessel.
	 */
	virtual void PreStep(double simdt);

	/**
	* \brief is called at the end of every clbkPreStep on the vessel.
	* This essentially serves the purpose to process the loopback pipe
	* of the eventhandler. It is executed after everybody has done its thing
	* and sent all its messages, and is used to finalise the vessel state
	* before Orbiter applies it.It should NOT be overriden by inheriting classes.
	* If you push events to the waiting queue, this is the time they will fire,
	* and you get to finalise your states by reacting to them.
	*/
	void PreStateUpdate();

protected:
	IMS2 *vessel;
	
};

