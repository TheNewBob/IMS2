#pragma once
class IMS_ModuleFunction_Pressurised;
class IMS_Location;

// TODO Serialisation interface should be moved here, but it will break backwards compatibility. Do it when you have to break it anyways!

/**
 * \brief Abstract base class for objects capable of moving around between modules, such as storables and crew
 */
class IMS_Movable : public EventHandler
{
	friend class IMS_Location;
public:
	/**
 	 * \param location The module instance the movable is initially a part of
 	 * \param fixed Whether the movable can be moved from module to module
 	 */
	IMS_Movable(IMS_Location *location, bool fixed = false);
	
	virtual ~IMS_Movable();

	virtual double GetMass() { return mass; };
	virtual void PreStep(IMS_Location *location, double simdt) = 0;
	void Move(IMS_Location *from, IMS_Location *to);

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


protected:
	bool fixedpos = false;
	double mass;
	
	/**
	 * \brief takes all necessary steps to remove a movable from a module
	 * \param module The module instance to remove the movable from
	 */
	void removeFrom(IMS_Location *location);
	
	/**
	 * \brief takes all necessary steps to add a movable to a module
	 * \param module The module instance to which to add the movable
	 */
	void addTo(IMS_Location *location);

};

