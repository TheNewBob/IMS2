#pragma once
#include "EventHandler.h"
class IMS_Manager_Base :
	public EventHandler
{
public:
	IMS_Manager_Base(IMS2 *vessel);
	~IMS_Manager_Base();

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

