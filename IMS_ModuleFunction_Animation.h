#pragma once


class IMS_Animation_Base;

class IMS_ModuleFunction_Animation :
	public IMS_ModuleFunction_Base
{
public:
	IMS_ModuleFunction_Animation(IMS_ModuleFunctionData_Animation *_data, IMS_Module *_module);
	~IMS_ModuleFunction_Animation();

	virtual void PostLoad(){};
	virtual void SaveState(FILEHANDLE scn);
	virtual void AddFunctionToVessel(IMS2 *vessel);
	virtual void RemoveFunctionFromVessel(IMS2 *vessel);
	virtual void PreStep(double simdt, IMS2 *vessel);

protected:
	virtual bool ProcessEvent(Event_Base *e);
	map<string, IMS_Animation_Base*> animations;
	virtual bool processScenarioLine(string line);
	void addAnimationsToVessel();
	void enableAnimations();
	void disableAnimations();
	/* returns false if the passed animation is prevented from starting
	 * by one of its dependencies
	 */
	bool canStart(IMS_Animation_Base *anim, double speed);

	//whether the animations currently physically exist on a vessel or not
	bool animationsexist = false;

private:
	IMS_ModuleFunctionData_Animation *data;


};

