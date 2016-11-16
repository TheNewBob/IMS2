#include "Common.h"
#include "Managers.h"
#include "IMS.h"
#include "IMS_RcsManager.h"
#include "IMS_Autopilot_Base.h"
#include "IMS_Autopilot_Killrot.h"


IMS_Autopilot_Killrot::IMS_Autopilot_Killrot(IMS2 *vessel)
	: IMS_Autopilot_Base(vessel, AP_MODE::KILLROT)
{
}


IMS_Autopilot_Killrot::~IMS_Autopilot_Killrot()
{
}

/**
 * \brief attempts to cancel out a vessels rotation frame by frame.
 * \author Hlynkacg
 */
void IMS_Autopilot_Killrot::PreStep(double simdt)
{
	if (running)
	{
		// Declarations
		VECTOR3 w, L, PMI;

		// Get angular velocity and PMI
		vessel->GetAngularVel(w);
		vessel->GetPMI(PMI);

		// Orbiter saves PMI as a normalized value we need to "de-normalise" it.
		PMI *= vessel->GetMass();

		// Multiply velocity around each axis by corrisponding PMI to get angular momentum.
		L.x = w.x * PMI.x;
		L.y = w.y * PMI.y;
		L.z = w.z * PMI.z;

		// Apply counter force
		rcs->SetCommandedTorque(-L / 5.0);
	}
}
