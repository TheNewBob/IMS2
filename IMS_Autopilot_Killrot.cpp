#include "Common.h"
#include "Managers.h"
#include "IMS.h"
#include "IMS_RcsManager.h"
#include "IMS_AutopilotOperation_Base.h"
#include "IMS_AutopilotOperation_Killrot.h"
#include "IMS_Autopilot_Base.h"
#include "IMS_Autopilot_Killrot.h"


IMS_Autopilot_Killrot::IMS_Autopilot_Killrot(IMS2 *vessel)
	: IMS_Autopilot_Base(vessel)
{
	operations[0] = new IMS_AutopilotOperation_Killrot(vessel);
}


IMS_Autopilot_Killrot::~IMS_Autopilot_Killrot()
{
}

 
void IMS_Autopilot_Killrot::PreStep(double simdt)
{
	if (running)
	{
		if (operations[0]->Process(simdt))
		{
			//operation has finished, disengage killrot ap
			vessel->DeactivateNavmode(NAVMODE_KILLROT);
			//magically set the vessels angular velocity to absolute zero.
			//this doesn't matter so much in terms of realism, as the angular velocity at this point
			//is well below 1e-6, but it's very nice for stability during high time acceleration.
			//and once we get thermodynamics, that stability might well be essential.
			vessel->SetAngularVel(_V(0, 0, 0));
			running = false;
		}
	}
}
