#include "Common.h"
#include "Managers.h"
#include "IMS.h"
#include "IMS_RcsManager.h"
#include "IMS_AutopilotOperation_Base.h"
#include "IMS_AutopilotOperation_Killrot.h"


IMS_AutopilotOperation_Killrot::IMS_AutopilotOperation_Killrot(IMS2 *vessel)
	: IMS_AutopilotOperation_Base(vessel)
{
}


IMS_AutopilotOperation_Killrot::~IMS_AutopilotOperation_Killrot()
{
}

/**
* \brief attempts to cancel out a vessels rotation frame by frame.
* \author Hlynkacg
*/
bool IMS_AutopilotOperation_Killrot::Process(double simdt)
{
	if (rcs->GetIntelligentRcs())
	{
		// Declarations
		VECTOR3 w, L, PMI;

		// Get angular velocity and PMI
		vessel->GetAngularVel(w);
		double angularvel = length2(w);

		if (angularvel < 0.00000001)
		{
			//we're hitting diminishing returns already, so let's quit at "quite good enough".
			return true;
		}

		vessel->GetPMI(PMI);

		// Orbiter saves PMI as a normalized value we need to "de-normalise" it.
		PMI *= vessel->GetMass();

		// Multiply velocity around each axis by corrisponding PMI to get angular momentum.
		L.x = w.x * PMI.x;
		L.y = w.y * PMI.y;
		L.z = w.z * PMI.z;

		// Apply counter force
		rcs->SetCommandedTorque(-L);
	}
	return false;
}