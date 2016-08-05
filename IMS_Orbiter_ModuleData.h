#pragma once
//this class contains static orbiter related properties that are common to all modules
class IMS_Orbiter_ModuleData
{
public:
	IMS_Orbiter_ModuleData();
	~IMS_Orbiter_ModuleData();
	bool hadErrors();
	virtual void LoadFromFile(string configfilename, IMSFILE file);

	string getMeshName();
	int getSize();
	string getConfigFileName();
	string getClassName();
	vector <IMSATTACHMENTPOINT> &getAttachmentPoints();

protected:
	std::string _configFileName;
	string _meshName;
	int _size;
	vector<IMSATTACHMENTPOINT> _attachmentPoints;
	bool _valid;

};

