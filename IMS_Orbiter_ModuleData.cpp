#include "Common.h"
#include "SimpleShape.h"
#include "IMS_Orbiter_ModuleData.h"
#include "Rotations.h"


IMS_Orbiter_ModuleData::IMS_Orbiter_ModuleData()
	: _meshName(""), _configFileName(""), _size(-1), _valid(false)
{
}

IMS_Orbiter_ModuleData::~IMS_Orbiter_ModuleData()
{
	if (hullshape != NULL)
	{
		delete hullshape;
	}
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

void IMS_Orbiter_ModuleData::getPmi(VECTOR3 &pmi)
{
	pmi = this->pmi;
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
		if (tokens[0].compare("inertia") == 0 && tokens.size() == 4)
		{
			pmi = _V(atof(tokens[1].data()),
					 atof(tokens[2].data()),
					 atof(tokens[3].data()));
		}
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
		else if (tokens[0].compare("begin_shape") == 0)
		{
			try
			{
				HULLSHAPEDATA hullshapedata = readShape(file);
				createHullShape(hullshapedata);
			}
			catch (invalid_argument e)
			{
				//the shape definition in the config file is invalid!
				Helpers::writeToLog(string(e.what()), L_WARNING);
			}
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


HULLSHAPEDATA IMS_Orbiter_ModuleData::readShape(IMSFILE file)
{
	HULLSHAPEDATA retdata;
	vector<string> tokens;
	while (Helpers::readLine(file, tokens, " \t="))
	{
		//check if line is valid
		if (tokens.size() == 0) continue;
		//put identifier in lowercase
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);


		if (tokens[0].compare("shape") == 0)
		{
			//check if the line is valid for any kind of shape
			if (tokens.size() < 4 || tokens.size() > 5)
			{
				Helpers::writeToLog(string("No shape type takes less than two parameters, or more than three!"), L_ERROR);
				throw(invalid_argument("Failed to read shape block, unable to construct hullshape for module"));
			}

			if (tokens[1].compare("cylinder") == 0)
			{
				retdata.shape = tokens[1];
				retdata.shapeparams.x = atof(tokens[2].data());
				retdata.shapeparams.y = atof(tokens[3].data());
				if (tokens.size() > 4)
				{
					//a number of segments has been defined
					retdata.shapeparams.z = atof(tokens[4].data());
				}
				else
				{
					//define 6 segments as default
					retdata.shapeparams.z = 6;
				}
			}
			else if (tokens[1].compare("box") == 0)
			{
				retdata.shape = tokens[1];
				retdata.shapeparams.x = atof(tokens[2].data());
				retdata.shapeparams.y = atof(tokens[3].data());
				retdata.shapeparams.z = atof(tokens[3].data());
			}
			else if (tokens[1].compare("sphere") == 0)
			{
				retdata.shape = tokens[1];
				retdata.shapeparams.x = atof(tokens[2].data());
				if (tokens.size() > 3)
				{
					//a number of segments has been defined
					retdata.shapeparams.y = atof(tokens[3].data());
				}
				else
				{
					//define 6 segments as default
					retdata.shapeparams.y = 6;
				}
			}
			else
			{
				//the shape type does not match any defined types
				Helpers::writeToLog(string(tokens[1] + "is not a valid shape type!"), L_ERROR);
				throw(invalid_argument("Failed to read shape block, unable to construct hullshape for module"));
			}
		}
		else if (tokens[0].compare("offset") == 0)
		{
			if (tokens.size() != 4)
			{
				Helpers::writeToLog(string("Shape offset must have format \"offset = x y z\"!"), L_ERROR);
				throw(invalid_argument("Failed to read shape block, unable to construct hullshape for module"));
			}
			try
			{
				retdata.pos = _V(atof(tokens[1].data()),
					atof(tokens[2].data()),
					atof(tokens[3].data()));
			}
			catch (exception e)
			{
				throw(invalid_argument(string("Failed to read shape block, unable to construct hullshape for module. Cause: " + string(e.what()))));
			}
		}
		else if (tokens[0].compare("scale") == 0)
		{
			if (tokens.size() != 4)
			{
				Helpers::writeToLog(string("Shape offset must have format \"scale = x y z\"!"), L_ERROR);
				throw(invalid_argument("Failed to read shape block, unable to construct hullshape for module"));
			}
			try
			{
				retdata.scale = _V(atof(tokens[1].data()),
					atof(tokens[2].data()),
					atof(tokens[3].data()));
			}
			catch (exception e)
			{
				throw(invalid_argument(string("Failed to read shape block, unable to construct hullshape for module. Cause: " + string(e.what()))));
			}
		}
		else if (tokens[0].compare("orientation") == 0)
		{
			if (tokens.size() != 7)
			{
				Helpers::writeToLog(string("Shape offset must have format \"orientation = x y z x y z\" (dir rot)!"), L_ERROR);
				throw(invalid_argument("Failed to read shape block, unable to construct hullshape for module"));
			}
			try
			{
				retdata.dir = _V(atof(tokens[1].data()),
					atof(tokens[2].data()),
					atof(tokens[3].data()));

				retdata.rot = _V(atof(tokens[4].data()),
					atof(tokens[5].data()),
					atof(tokens[6].data()));
			}
			catch (exception e)
			{
				throw(invalid_argument(string("Failed to read shape block, unable to construct hullshape for module. Cause: " + string(e.what()))));
			}
		}
		else if (tokens[0].compare("end_shape") == 0)
		{
			if (retdata.shape == "")
			{
				//Helpers::
				throw(invalid_argument(string("No shape defined in shape block!")));
			}
			return retdata;
		}
		tokens.clear();
	}

	throw(invalid_argument("END_SHAPE not defined!"));
}


void IMS_Orbiter_ModuleData::createHullShape(HULLSHAPEDATA shapedata)
{
	//create the basic shape depending on what shape is asked for
	if (shapedata.shape == "cylinder")
	{
		hullshape = new SimpleShape(SimpleShape::Cylinder(
			shapedata.shapeparams.x, shapedata.shapeparams.y, (UINT)shapedata.shapeparams.z));
	}
	else if (shapedata.shape == "box")
	{
		hullshape = new SimpleShape(SimpleShape::Box(
			shapedata.shapeparams.x, shapedata.shapeparams.y, shapedata.shapeparams.z));
	}
	else if (shapedata.shape == "sphere")
	{
		hullshape = new SimpleShape(SimpleShape::Sphere(
			shapedata.shapeparams.x, (UINT)shapedata.shapeparams.z));
	}

	//transform the shape into the final shape demanded by the config
	MATRIX3 shaperotation = Rotations::GetRotationMatrixFromOrientation(shapedata.dir, shapedata.rot);
	hullshape->Rotate(shaperotation);
	hullshape->Scale(shapedata.scale);
	hullshape->Translate(shapedata.pos);
}
