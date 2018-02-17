#pragma once
class IMS_Component_Model_Base
{
public:
	IMS_Component_Model_Base();
	~IMS_Component_Model_Base();

	virtual bool LoadFromFile(string configfile, IMSFILE cfg);
	double GetVolume();

protected:
	double volume = -1; 
	string name = "";
	
	bool processConfigLine(vector<string> &tokens);
	
};

