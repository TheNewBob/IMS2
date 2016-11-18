#pragma once

/**
 * \brief Simple Killrot AP implementation. 
 * Override Orbiters stock killrot, which doesn't handle IMS vessels too well
 *	since it can't see the thrusters true capabilities.
 */
class IMS_Autopilot_Killrot :
	public IMS_Autopilot_Base
{
public:
	IMS_Autopilot_Killrot(IMS2 *vessel);
	~IMS_Autopilot_Killrot();

	void PreStep(double simdt);

};

