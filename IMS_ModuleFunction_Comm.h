#pragma once

//the different states the module function can have
enum
{
	COMMSTATE_RETRACTED = 0,							//antenna is folded up
	COMMSTATE_DEPLOYED = 1,								//antenna is deployed
	COMMSTATE_DEPLOYING = 2,							//antenna is in the process of deploying
	COMMSTATE_RETRACTING = 3,							//antenna is in the process of retracting
	COMMSTATE_SEARCHING = 4,							//antenna is in the search animation
	COMMSTATE_STOP_SEARCHING = 5,						//antenna has stoped searching and is waiting for animation to reach origin
	COMMSTATE_ALIGNING = 6,								//antenna has been assigned a target and is aligning
	COMMSTATE_TRACKING = 7,								//antenna is aligned and tracking the target
	COMMSTATE_STOP_TRACKING = 8							//antenna has stoped tracking and is waiting for animation to revert to origin
};

class IMS_ModuleFunctionData_Comm;
class GUI_ModuleFunction_Comm;

/* callback to set the target of a comm function
*/
bool SetTargetInputCallback(void *id, char *str, void *usrdata);


class IMS_ModuleFunction_Comm :
	public IMS_ModuleFunction_Base
{


public:
	/* creates a new communications module function
	 * _data: the configfile data on which to build this module function
	 * _module: the module instance this module function is added to
	 * creategui: pass false if you don't want the module function to create its default GUI
     *	this is only intended to be used by classes that will inherit from this
	 */
	IMS_ModuleFunction_Comm(IMS_ModuleFunctionData_Comm *_data, IMS_Module *_module, bool creategui = true);
	~IMS_ModuleFunction_Comm();

	//overloads from IMS_ModuleFunction_Base
	virtual void PostLoad();
	virtual void SaveState(FILEHANDLE scn);
	virtual void AddFunctionToVessel(IMS2 *vessel);
	virtual void RemoveFunctionFromVessel(IMS2 *vessel){};
	virtual void PreStep(double simdt, IMS2 *vessel);
	virtual GUI_ModuleFunction_Base *GetGui();


	//functions to communicate user input to the module
	virtual void CommandDeploy();
	virtual void CommandSearch();
	virtual void CommandTrack();
	virtual void CommandSetTarget();
	virtual void SetTarget(string target);

	IMS_ModuleFunctionData_Comm *GetData(){ return data; };

protected:
	string animationstarted = "";
	
	virtual bool processScenarioLine(string line);
	virtual bool ProcessEvent(Event_Base *e);

private:
	GUI_ModuleFunction_Comm *menu;
	IMS_ModuleFunctionData_Comm *data;
	StateMachine state;
	int lasttrackingstatus = -1;
	string targetname = "";

};

