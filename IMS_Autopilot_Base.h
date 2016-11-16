#pragma once

/**
* \brief Defines available AP modes.
*/
enum AP_MODE
{
	KILLROT,
};



/**
 * Base class for autopilot implementations, whether that be stock AP overrides or advanced custom autopilots.
 *
 * In order to add a new autopilot, inherit this class, define a new AP_MODE above, include your header in 
 * autopilot_includes.h, and add the autopilot instance in the constructor of IMS2.
 */
class IMS_Autopilot_Base
{
public:
	IMS_Autopilot_Base(IMS2 *vessel, AP_MODE mode) : vessel(vessel), mode(mode)
	{
		rcs = vessel->GetRcsManager();
	};

	~IMS_Autopilot_Base() {};

	virtual void SetActive(bool active) { running = active; };
	virtual void PreStep(double simdt) = 0;

protected:
	AP_MODE mode;
	bool running = false;
	IMS2 *vessel;
	IMS_RcsManager *rcs;
};

