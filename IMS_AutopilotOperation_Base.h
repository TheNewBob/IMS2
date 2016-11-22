#pragma once

/**
* Base class for autopilot operations.
*
* Autopilot operations should be small, encapsulated programs that execute one particular task.
* The idea here is that all the maneuvering logic is contained in these operations, 
* while an IMS_Autopilot is tasked with keeping track of state and invoking appropriate operations
* at appropriate times.
* \note Operations should be stateless! It is the job of the containing IMS_Autopilot implementation to 
*	call the Process() method on an operation when needed.
*/
class IMS_AutopilotOperation_Base
{
public:
	IMS_AutopilotOperation_Base(IMS2 *vessel) : vessel(vessel)
	{
		rcs = vessel->GetRcsManager();
	};

	virtual ~IMS_AutopilotOperation_Base() {};

	/**
	 * \brief Controll loop that applies AP operation to vessel.
	 * \param simdt Timestep since last frame.
	 * \return True if the operation considers itself completed, false otherwise
	 */
	virtual bool Process(double simdt) = 0;

protected:
	IMS2 *vessel;
	IMS_RcsManager *rcs;
};
