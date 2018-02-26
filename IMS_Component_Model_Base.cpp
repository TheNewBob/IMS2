#include "Common.h"
#include "IMS_Component_Model_Base.h"


IMS_Component_Model_Base::IMS_Component_Model_Base(string type) : type(type)
{
}


IMS_Component_Model_Base::~IMS_Component_Model_Base()
{
}


bool IMS_Component_Model_Base::LoadFromFile(string configfile, IMSFILE cfg)
{
	vector<string> tokens;
	//split line into componentes by spaces, tabs and the equal sign
	while(Helpers::readLine(cfg, tokens, " \t="))
	{
		if (tokens.size() == 0) continue;

		//transform parameter identifier to lower case
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);
		try 
		{
			if (!processConfigLine(tokens))
			{
				Olog::warn("Unknown parameter in component config %s: %s", configfile.data(), tokens[0].data());
			}
		}
		catch (runtime_error e)
		{
			Olog::error("%s in %s", e.what(), configfile.data());
			throw(runtime_error("Fatal error while loading component config!"));
		}

		tokens.clear();
	}
	return true;
}

bool IMS_Component_Model_Base::processConfigLine(vector<string> &tokens)
{
	if (tokens[0].compare("name") == 0)
	{
		//the string might contain spaces, so let's put it together again
		for (UINT i = 1; i < tokens.size(); ++i)
		{
			if (i > 1)
			{
				name += " ";
			}
			name += tokens[i];
		}
		return true;
	}
	else if (tokens[0].compare("volume") == 0)
	{
		volume = Helpers::stringToDouble(tokens[1]);
		return true;
	}
	else if (tokens[0].compare("emptymass") == 0)
	{
		emptymass = Helpers::stringToDouble(tokens[1]);
		return true;
	}

	return false;
}

bool IMS_Component_Model_Base::Validate(string configname)
{
	bool valid = true;
	if (name.compare("") == 0)
	{
		Olog::warn("No name defined in component %s", configname.data());
		valid = false;
	}
	if (emptymass < 0) 
	{
		Olog::warn("No valid EmptyMass parameter in component %s", configname.data());
		valid = false;
	}
	if (volume < 0)
	{
		Olog::warn("No valid volume defined in component %s", configname.data());
		valid = false;
	}
	return valid;
}
