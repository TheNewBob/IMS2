#pragma once

class IMS_Component_Lighting :
	public IMS_Component_Base
{
public:
	IMS_Component_Lighting(IMS_Component_Model_Lighting *data, IMS_Location *location);
	~IMS_Component_Lighting();

	virtual string Serialize();
	virtual void Deserialize(string data);
	virtual double GetMass();
	virtual void PreStep(IMS_Location *location, double simdt);

	virtual bool ProcessEvent(Event_Base *e);

protected:
	virtual void getDynamicData(map<string, string> &keysAndValues);
	virtual void setDynamicData(map<string, string> &keysAndValues);

};

