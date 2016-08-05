#pragma once

struct ANIMCOMPONENTDATA
{
	vector<UINT> groups;
	string type = "";
	VECTOR3 reference;
	VECTOR3 axis;
	float range;
	int parent;
	double duration[2];
};

/**
 * \brief defines a dependency for an animation.
 *
 * a dependency is a set state of another animation
 * that must be fulfilled in order for this animation
 * to be able to execute
 */
struct ANIMATIONDEPENDENCY
{
	int direction;					//for which direction of THIS animation the dependency applies (<0 reverse, >1 normal)
	string dependencyid;			//the identifier of the animation this animation depends on
	double dependencystate;			//the state of the other animation this animation depends on
};

struct ANIMATIONDATA
{
	string id = "";
	double duration;
	string type = "";
	vector<ANIMCOMPONENTDATA> components;
	vector<ANIMATIONDEPENDENCY> dependencies;
};

class IMS_ModuleFunctionData_Animation :
	public IMS_ModuleFunctionData_Base
{
	friend class IMS_ModuleFunction_Animation;

public:
	IMS_ModuleFunctionData_Animation();
	~IMS_ModuleFunctionData_Animation();

	/* overloading the loader function, because this modulefunction needs direct file access
	*/
	bool LoadFromFile(string configfile, IMSFILE cfg);

private:

	ANIMATIONDATA loadAnimation(string id, IMSFILE cfg);
	ANIMCOMPONENTDATA loadAnimComponent(IMSFILE cfg);
	vector<ANIMATIONDATA> animations;

	bool processConfigLine(vector<string> &tokens){ return false; };
	bool validateData(string configfile){ return true; };

};

