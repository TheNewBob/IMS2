#pragma once


/**
 * Base class for autopilot implementations, whether that be stock AP overrides or advanced custom autopilots.
 *
 * In order to add a new autopilot, inherit this class, define a new AP_MODE and include your header in 
 * autopilot_includes.h, and add the autopilot instance in the constructor of IMS2.
 */
class IMS_Autopilot_Base
{
public:
	IMS_Autopilot_Base(IMS2 *vessel) : vessel(vessel) {};

	~IMS_Autopilot_Base() 
	{
		for (auto i = operations.begin(); i != operations.end(); ++i)
		{
			delete i->second;
		}
	};

	/**
	 * \brief Engages or disengages the autopilot.
	 * \param active If true, the AP will be engaged, if false disengaged.
	 */
	virtual void SetActive(bool active) { running = active; };
	
	/**
	 * \brief Called by IMS2 every frame. Use to manage autopilot state and call operations.
	 * \param simdt Simtime elapsed since the last frame;
	 */
	virtual void PreStep(double simdt) = 0;

protected:

	bool running = false;
	IMS2 *vessel;
	map<int, IMS_AutopilotOperation_Base*> operations;		//!< Used to store an instance of all operation this autopilot uses.
};

