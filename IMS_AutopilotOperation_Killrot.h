/**
* Base class for autopilot implementations, whether that be stock AP overrides or advanced custom autopilots.
*
* In order to add a new autopilot, inherit this class, define a new AP_MODE above, include your header in
* autopilot_includes.h, and add the autopilot instance in the constructor of IMS2.
* \author AP-code by Hlynkacg, encapsulation by Jedidia.
*/
class IMS_AutopilotOperation_Killrot : public IMS_AutopilotOperation_Base
{
public:
	IMS_AutopilotOperation_Killrot(IMS2 *vessel);

	~IMS_AutopilotOperation_Killrot();

	bool Process(double simdt);

};
