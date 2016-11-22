#pragma once
class IMS_ModuleFunction_Pressurised;
class IMS_Location;

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
	virtual void PreStep(IMS_Location *location) = 0;
	void Move(IMS_Location *from, IMS_Location *to);

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

