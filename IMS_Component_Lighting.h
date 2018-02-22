#pragma once

class IMS_Component_Lighting :
	public IMS_Component_Base
{
public:
	IMS_Component_Lighting(IMS_Component_Model_Lighting *data, IMS_Module *module);
	~IMS_Component_Lighting();

	virtual string Serialize();
	virtual void Deserialize(string data);
	virtual double GetMass();
	virtual void PreStep(double simdt, IMS2 *vessel);

	virtual bool ProcessEvent(Event_Base *e);
};

