#pragma once

struct LOCATION_CONTEXT;

class IMS_Component_Model_Base
{
public:
	IMS_Component_Model_Base(string type);
	~IMS_Component_Model_Base();

	virtual bool LoadFromFile(string configfile, IMSFILE cfg);
	virtual bool Validate(string configname);

	double GetVolume() { return volume; };
	double GetEmptyMass() { return emptymass; };
	string GetType() { return type; };
	string GetName() { return name; };
	vector<LOCATION_CONTEXT> &GetContexts() { return contexts; };

protected:
	double volume = -1; 
	double emptymass = -1;
	string name = "";
	string type = "";
	string description = "";
	vector<LOCATION_CONTEXT> contexts;
	
	bool processConfigLine(vector<string> &tokens);
	
};

