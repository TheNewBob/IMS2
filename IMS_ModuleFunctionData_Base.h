#pragma once

/**
 * \brief Abstract base class for storing generic static data of module functions loaded from a config file
 *
 * Every derivative of IMS_ModuleFunction_Base() needs a corresponding derivative of this class
 * to load and store its configfile data.
 *
 * Module functions are specific abilities that can be added to a module in a config file, like thrusters,
 * tanks, habitat etc. Many modules will have only one module function, but an arbitrary number of 
 * modulefunctions can be defined for any module.
 *
 * <h3> Config structure: </h3>
 * An arbitrary number of module functions can be defined for any module.
 * Any module function must be enclosed in a module function block like this:
 * \code
 * BEGIN_MODULEFUNCTION <modulefunction-ID>
 *		<modulefunction specific parameter>
 *		 .
 *		 .	
 * END_MODULEFUNCTION
 * \endcode
 * For a list of valid modulefunction ids, see the documentation for IMS_ModuleFunctionFactory.h
 *
 * <h4> Generic modulefunction parameters </h4>
 * These parameters can be added to any module function
 * \code
 * name = <string name>				//a name for this module function type that will be used in various displays in the UI
 * \endcode
 * \see IMS_ModuleFunctionFactory.h
 */
class IMS_ModuleFunctionData_Base
{
public:
	IMS_ModuleFunctionData_Base() { type = MTYPE_NONE; };
	~IMS_ModuleFunctionData_Base() {};

	/**
	 * \brief Loads the data from config file.
	 * \param configfile The name of the config file
	 * \param cfg The config file itself, wrapped in an IMSFILE structure for oapi independency
	 * \note This function sends all lines concerning this ModuleFunction to processConfigLine() one by one.
	 *	In general, it will be much easier to overload processConfigLine(). Overload this function only if 
	 *	you cannot practically load your data on a pure line by line basis.
	 * \return True if loading was succesful, false otherwise
	 * \see processConfigLine()
	 */
	virtual bool LoadFromFile(string configfile, IMSFILE cfg)
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
			}
			else if (tokens[0].compare("end_modulefunction") == 0)
			{
				endofblock = true;
			}
			else
			{
				try
				{
					processConfigLine(tokens);
				}
				catch (runtime_error e)
				{
					//add the config file to the error message
					Helpers::writeToLog(string(e.what()) + " in " + configfile, L_ERROR);
				}

			}
			tokens.clear();
		}

		if (!validateData(configfile))
		{
			//do not catch this exception! It's here to prevent IMS from running if there are severe errors in a config file
			throw runtime_error("IMS2: CRITICAL ERROR in " + configfile + "! crashing gracefully to desktop, see orbiter.log for details");
		}
		return true;
	};

	/**
	 * \brief This function serves to check whether a module function has a GUI before creating it.
	 *
	 * \note If your ModuleFunction has its own GUI, overload this function and return true, as simple as that.
	 * \return Always false
	 */
	virtual bool HasGui() { return false; };

	/**
	 * \return The type of the ModuleFunction this data is for
	 */
	FUNCTIONTYPE GetType() { return type; };
	
	/**
	 * \return The name of this ModuleFunction
	 */
	string GetName() { return name; };

protected:
	FUNCTIONTYPE type;								//!< Stores the type of the ModuleFunction
	string name = "";								//!< Stores the Name of the ModuleFunction

	/**
	 * \brief Receives all lines of the config file that concern this ModuleFunction one by one
	 *
	 * Overload this function to receive the relevant data of your module.
	 * \param tokens a vector of strings that contains the individual parts of the line, delimited by whitespace or '='
	 * \note Throw runtime_error with an error message if you encounter an invalid line! 
	 *	The message will be loged in Orbiter.log together with the name of the config file.
	 * \return true if the passed line was processed, false if not.
	 */
	virtual bool processConfigLine(vector<string> &tokens) = 0;
	
	/**
	 * \brief Validates the data for completeness after loading
	 *
	 * Implement this function to validate your data at the end of the loading
	 * process. Check if everything's there that needs to be, and write log messages
	 * if it isn't.
	 * \param configfile The name of the configfile, so you can add it to your messages so config writers find their mistakes faster.
	 * \return False if the modulefunction is unable to function, otherwise return true.
	 * \note If your Implementation returns false, The loading process will be aborted by forcing orbiter to crash, 
	 * so be sure to keep your loging straight!
	 */
	virtual bool validateData(string configfile) = 0;
};

