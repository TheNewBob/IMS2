#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Animation.h"


IMS_ModuleFunctionData_Animation::IMS_ModuleFunctionData_Animation()
{
	type = MTYPE_ANIM;
}


IMS_ModuleFunctionData_Animation::~IMS_ModuleFunctionData_Animation()
{
}


bool IMS_ModuleFunctionData_Animation::LoadFromFile(string configfile, IMSFILE cfg)
{

	//read until the end of the MODULEFUNCTION block
	bool endofblock = false;
	vector<string> tokens;
	while (!endofblock)
	{
		//split line into componentes by spaces, tabs and the equal sign
		Helpers::readLine(cfg, tokens, " \t=");
		if (tokens.size() == 0) continue;

		//transform parameter identifier to lower case
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

		//check if line is beginning of a new animation
		if (tokens[0].compare("begin_anim") == 0)
		{
			if (tokens.size() > 1)
			{
				try
				{
					animations.push_back(loadAnimation(tokens[1], cfg));
				}
				catch (runtime_error e)
				{
					Olog::error("%s in %s", e.what(), configfile);
					return false;
				}
			}
		}

		else if (tokens[0].compare("end_modulefunction") == 0)
		{
				endofblock = true;
		}
		tokens.clear();
	}


	return true;
}


ANIMATIONDATA IMS_ModuleFunctionData_Animation::loadAnimation(string id, IMSFILE cfg)
{
	ANIMATIONDATA a;
	a.id = id;

	bool endofblock = false;
	bool durationdefined = false;
	bool typedefined = false;
	vector<string> tokens;
	while (!endofblock)
	{
		//split line into componentes by spaces, tabs and the equal sign
		Helpers::readLine(cfg, tokens, " \t=");
		if (tokens.size() == 0) continue;
		//transform parameter identifier to lower case
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

		if (tokens.size() > 0)
		{
			//check if line is the end of the block
			if (tokens[0].compare("end_anim") == 0)
			{
				endofblock = true;
				continue;
			}
			else if (tokens[0].compare("duration") == 0)
			{
				a.duration = atof(tokens[1].data());
				durationdefined = true;
			}
			else if (tokens[0].compare("dependency") == 0)
			{
				if (tokens.size() != 4)
				{
					//the line must consist of the tag, the direction, the id and the state
					throw runtime_error("ERROR: invalid dependency declaration!");
				}
				//add a new dependency to the animation
				ANIMATIONDEPENDENCY newdep;
				newdep.direction = atoi(tokens[1].data());
				newdep.dependencyid = tokens[2].data();
				newdep.dependencystate = atof(tokens[3].data());
				a.dependencies.push_back(newdep);
			}
			else if (tokens[0].compare("type") == 0)
			{
				transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::tolower);
				//verify the animation type
				if (tokens[1] == "sequence" ||
					tokens[1] == "continuous")
				{
					a.type = tokens[1];
				}
				else if (tokens[1].compare("track") == 0)
				{
					//it's a tracking animation, which means the facing will be declared immediately afterwards
					if (tokens.size() < 5)
					{
						//the facing has not been corretly declared
						throw runtime_error("ERROR: tracking animation has invalid facing");
					}
					else
					{
						//compose the type name of the type as well as the facing
						a.type = tokens[1] + " " + tokens[2] + " " + tokens[3] + " " + tokens[4];
					}
				}
				else
				{
					throw runtime_error("ERROR: Unknown animation type \"" + tokens[1] + "\"");
				}
			}
			else if (tokens[0].compare("begin_comp") == 0)
			{
				//an animation component is declared
				try
				{
					a.components.push_back(loadAnimComponent(cfg));
					typedefined = true;
				}
				catch (runtime_error e)
				{
					throw e;
				}
			}
		}
		tokens.clear();
	}
	if (!durationdefined)
	{
		throw runtime_error("ERROR: Animation has no duration defined");
	}
	if (!typedefined)
	{
		throw runtime_error("ERROR: No animation type defined");
	}
	return a;
}

ANIMCOMPONENTDATA IMS_ModuleFunctionData_Animation::loadAnimComponent(IMSFILE cfg)
{
	ANIMCOMPONENTDATA a;
	a.parent = -1;
	//some flags for verifying whether the animation was defined correctly
	bool groupsdefined = false;
	bool referencedefined = false;
	bool axisdefined = false;
	bool rangedefined = false;
	bool durationdefined = false;


	bool endofblock = false;
	vector<string> tokens;
	while (!endofblock)
	{
		//split line into componentes by spaces, tabs and the equal sign
		Helpers::readLine(cfg, tokens, " \t=");
		//transform parameter identifier to lower case
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

		//check if the line is valid
		if (tokens.size() < 2)
		{
			//check if line is the end of the block
			if (tokens[0].compare("end_comp") == 0)
			{
				endofblock = true;
				continue;
			}
			else
			{
				//the line is not valid
				throw runtime_error("ERROR: parameter \"" + tokens[0] + "\" has no value");
			}
		}
		if (tokens[0].compare("end_step") == 0)
		{

		}
		else if (tokens[0].compare("groups") == 0)
		{
			//read the groups
			for (UINT i = 1; i < tokens.size(); ++i)
			{
				a.groups.push_back(atoi(tokens[i].data()));
				groupsdefined = true;
			}
		}
		else if (tokens[0].compare("type") == 0)
		{
			if (tokens.size() < 2)
			{
				throw runtime_error("ERROR: No animation type defined");
			}
			transform(tokens[1].begin(), tokens[1].end(), tokens[1].begin(), ::tolower);
			
			//check if the animation type is valid
			if (tokens[1].compare("rotate") == 0 ||
				tokens[1].compare("translate") == 0 ||
				tokens[1].compare("scale") == 0)
			{
				a.type = tokens[1];
			}
			else
			{
				throw runtime_error("ERROR: invalid animation type \"" + tokens[1] + "\"");
			}
		}
		//origin and translate are exchangable and only provided for synthax convienience. You need either one or the other, so they may as well be the same under the hood
		else if (tokens[0].compare("origin") == 0 || tokens[0].compare("translate") == 0)
		{
			try
			{
				a.reference = Helpers::parseVector3Parameter(tokens);
				referencedefined = true;
			}
			catch (invalid_argument e)
			{
				throw runtime_error(string(e.what()));
			}
		}
		//axis and scale are exchangable and only provided for synthax convienience. You need either one or the other, so they may as well be the same under the hood
		else if (tokens[0].compare("axis") == 0 || tokens[0].compare("scale") == 0)
		{
			try
			{
				a.axis = Helpers::parseVector3Parameter(tokens);
				axisdefined = true;
			}
			catch (invalid_argument e)
			{
				throw runtime_error(string(e.what()));
			}
		}
		else if (tokens[0].compare("range") == 0)
		{
			a.range = (float)atof(tokens[1].data());
			rangedefined = true;
		}
		else if (tokens[0].compare("parent") == 0)
		{
			a.parent = atoi(tokens[1].data());
		}
		else if (tokens[0].compare("duration") == 0)
		{
			if (tokens.size() < 3)
			{
				throw runtime_error("Error: duration needs 2 values");
			}
			a.duration[0] = atof(tokens[1].data());
			a.duration[1] = atof(tokens[2].data());
			durationdefined = true;
		}
		tokens.clear();
	}


	//validate the data for completeness
	if (a.type == "" || !groupsdefined ||
		(a.type == "rotation" || a.type == "scale") && (!referencedefined || !axisdefined) ||
		a.type == "translation" && !referencedefined)
	{
		throw runtime_error("ERROR: incomplete parameter set for animation component");
	}
	if (!durationdefined)
	{
		//set default value from 0 to 1
		a.duration[0] = 0.0;
		a.duration[1] = 1.0;
	}
	return a;
}