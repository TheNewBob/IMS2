#pragma once
class IMS_Component_Model_Base
{
public:
	IMS_Component_Model_Base();
	~IMS_Component_Model_Base();

	virtual bool LoadFromFile(string configfile, IMSFILE cfg);
	virtual bool Validate(string configname);

	double GetVolume() { return volume; };
	double GetEmptyMass() { return emptymass; };

protected:
	double volume = -1; 
	double emptymass = -1;
	string name = "";
	
	bool processConfigLine(vector<string> &tokens);
	
};

