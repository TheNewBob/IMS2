#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"


IMS_Orbiter_ModuleData::IMS_Orbiter_ModuleData()
	: _meshName(""), _configFileName(""), _size(-1), _valid(false)
{
}

IMS_Orbiter_ModuleData::~IMS_Orbiter_ModuleData()
{
}

bool IMS_Orbiter_ModuleData::hadErrors()
{
	//return if there were errors while loading the file
	return !_valid;
}

string IMS_Orbiter_ModuleData::getMeshName()
{
	return _meshName;
}

int IMS_Orbiter_ModuleData::getSize()
{
	return _size;
}

string IMS_Orbiter_ModuleData::getConfigFileName()
{
	return _configFileName;
}

//returns the classname of a vessel, basically the config file name without extension
string IMS_Orbiter_ModuleData::getClassName()
{
	string::size_type endofname = _configFileName.find_last_of(".");
	string classname = _configFileName.substr(0, endofname);
	return classname;
}

vector <IMSATTACHMENTPOINT> &IMS_Orbiter_ModuleData::getAttachmentPoints()
{
	return _attachmentPoints;
}


void IMS_Orbiter_ModuleData::LoadFromFile(string configfilename, IMSFILE file)
//read the module config file
{
	_configFileName = configfilename;
	//reset the file to the start
	Helpers::resetFile(file);

	//read through the whole file
	vector<string> tokens;
	bool startAttachments = false;
	//split the line by whitespaces and equal sign
	while (Helpers::readLine(file, tokens, " \t="))
	{
		//check if line is valid
		if (tokens.size() == 0)
			continue;
		//put identifier in lowercase
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);
		if (tokens[0].compare("begin_ims_attachment") == 0)
		{
			startAttachments = true;
		}
		else if (tokens[0].compare(0, 18, "end_ims_attachment") == 0)
		{
			startAttachments = false;
		}
		else if (tokens[0].compare("meshname") == 0)
		{
			_meshName = tokens[1];
		}
		else if (tokens[0].compare("size") == 0)
		{
			_size = Helpers::stringToInt(tokens[1]);
		}


		//read IMS attachment points
		else if (startAttachments)
		{
			if (tokens.size() == 10)
			//line is a valid ims attachment definition
			{
				IMSATTACHMENTPOINT newAtt;

				newAtt.pos.x = atof(tokens[0].data());
				newAtt.pos.y = atof(tokens[1].data());
				newAtt.pos.z = atof(tokens[2].data());

				newAtt.dir.x = atof(tokens[3].data());
				newAtt.dir.y = atof(tokens[4].data());
				newAtt.dir.z = atof(tokens[5].data());

				newAtt.rot.x = atof(tokens[6].data());
				newAtt.rot.y = atof(tokens[7].data());
				newAtt.rot.z = atof(tokens[8].data());

				newAtt.id = tokens[9];
				newAtt.dockedVessel = NULL;
				newAtt.exists = true;
				newAtt.dockPort = NULL;
				newAtt.rotModifier = 0;
				newAtt.connectsTo = NULL;
				newAtt.isMemberOf = NULL;

				_attachmentPoints.push_back(newAtt);
			}
			else
			{
				Helpers::writeToLog(_configFileName + ": invalid IMS attachmentpoint definition", L_ERROR);
				return;
			}
		}
		tokens.clear();
	}
	if (_attachmentPoints.size() == 0)
	//there were no attachment points declared
	{
		Helpers::writeToLog(_configFileName + ": no IMS attachmentpoints declared", L_ERROR);
		return;
	}

	//the config has correctly loaded. We're not checking for a declared mesh, Orbiter already does that
	_valid = true;
}