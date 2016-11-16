#pragma once


class IMS_Autopilot_Killrot :
	public IMS_Autopilot_Base
{
public:
	IMS_Autopilot_Killrot(IMS2 *vessel);
	~IMS_Autopilot_Killrot();

	void PreStep(double simdt);

};

