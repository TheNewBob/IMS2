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
	* \brief is called on every clbkPostStep call on the vessel.
	*/
	virtual void PostStep(double simdt);

protected:
	IMS2 *vessel;
	
};

