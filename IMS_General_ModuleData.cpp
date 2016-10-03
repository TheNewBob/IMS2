#include "Common.h"
#include "SimpleShape.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Factory.h"
#include "Rotations.h"



IMS_General_ModuleData::IMS_General_ModuleData()
	: _mass(-1), _name("")
{
}


IMS_General_ModuleData::~IMS_General_ModuleData()
{
	if (hullshape != NULL)
	{
		delete hullshape;
	}
}


bool IMS_General_ModuleData::LoadFromFile(string configfile, IMSFILE cfg)
{
	//reset the file to the start
	Helpers::resetFile(cfg);

	//read through the whole file
	vector<string> tokens;
	bool startAttachments = false;
	//split the line by whitespaces and equal sign
	while (Helpers::readLine(cfg, tokens, " \t="))
	{
		//check if line is valid
		if (tokens.size() == 0) continue;
		//put identifier in lowercase
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

		if (tokens[0].compare("mass") == 0)
		{
			_mass = Helpers::stringToDouble(tokens[1]);
		}
		else if (tokens[0].compare("name") == 0)
		{
			_name = tokens[1];
		}
		else if (tokens[0].compare("begin_shape") == 0)
		{
			try
			{
				HULLSHAPEDATA hullshapedata = readShape(cfg);
				createHullShape(hullshapedata);
			}
			catch (invalid_argument e)
			{
				//the shape definition in the config file is invalid!
				Helpers::writeToLog(string(e.what()), L_WARNING);
			}
		}
		else if (tokens[0].compare("begin_modulefunction") == 0)
		{
			//the config file declared a module function. Create it and let it parse its parameters
			IMS_ModuleFunctionData_Base *data = IMS_ModuleFunction_Factory::CreateNewModuleFunctionData(
														IMS_ModuleFunction_Factory::GetFunctionTypeFromString(tokens[1]));
			if (data->LoadFromFile(configfile, cfg))
			{
				functiondata.push_back(data);
			}
		}
		tokens.clear();
	}
	
	//check if the module has at least one valid function definition
	if (functiondata.size() == 0)
	{
		Helpers::writeToLog(configfile + ": No valid module function defined!", L_ERROR);
		return false;
	}
	return true;
}

double IMS_General_ModuleData::getMass()
{
	return _mass;
}


string IMS_General_ModuleData::getName()
{
	return _name;
}

HULLSHAPEDATA IMS_General_ModuleData::readShape(IMSFILE file)
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


void IMS_General_ModuleData::createHullShape(HULLSHAPEDATA shapedata)
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
