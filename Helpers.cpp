#include "Common.h"

const string Helpers::whitespace = " \t";
LOGLEVEL Helpers::loglevel;

void Helpers::Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (pos != string::npos || lastPos != string::npos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

string Helpers::WriteTokenizedString(vector<string> &tokens)
{
	string retstring;
	for (UINT i = 0; i < tokens.size(); ++i)
	{
		if (i > 0)
		{
			retstring += " ";
		}
		retstring += tokens[i];
	}
	return retstring;
}

void Helpers::resetFile(IMSFILE file)
//resets the file pointer to the beginning of the file. Well, at least to the orbiter module declaration, which should be the beginning of any IMS module
{
	char l[500];
	oapiReadItem_string(file.file, "Module", l);
}

bool Helpers::readLine(IMSFILE file, std::vector<std::string>& tokens, const std::string &delimiters)
{
	char *l;
	std::string line;
	ifstream test;
	
	//return if we reached eof
	if (!oapiReadScenario_nextline(file.file, l))
		return false;
	line = l;
	//cut everything beyond a ';'
	if (line.find_first_of(';') != std::string::npos)
		line.erase(line.find_first_of(';'), std::string::npos);
	//remove extra spaces
	removeExtraSpaces(line);

	// Skipping delimiters at the beginning
	std::string::size_type lastPos = line.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = line.find_first_of(delimiters, lastPos);

	while (pos != std::string::npos || lastPos != std::string::npos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(line.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = line.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = line.find_first_of(delimiters, lastPos);
	}
	return true;
}

void Helpers::removeExtraSpaces(std::string& str)
{
/*	std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
	str.erase(new_end, str.end());*/

	size_t newbegin = str.find_first_not_of(whitespace);
	if (newbegin == string::npos)
	{
		//the passed string consists ONLY of whitespace
		str = "";
		return;
	}
	size_t range = str.find_last_not_of(whitespace) - newbegin + 1;
	
	str = str.substr(newbegin, range);
}

int Helpers::stringToInt(const std::string& inputString)
{
	int num;
	std::istringstream(inputString) >> num;
	return num;
}

double Helpers::stringToDouble(const std::string& inputString)
{
	double num;
	std::istringstream(inputString) >> num;
	return num;
}

string Helpers::doubleToString(double number, int digits)
{
	if (digits > -1)
	{
		//increment digits by 1, otherwise the digit passed is the digit that gets rounded, i.e. is not included in the string
		digits++;
		double magnitude = pow(10, digits);
		//round the double to the requested number of digits
		number = floor(number * magnitude + 0.5) / magnitude;
	}
	//write to stringstream and return resulting string
	std::ostringstream strs;
	strs.precision(digits - 1);
	strs << fixed << number;
	return strs.str();
}

string Helpers::doubleToScientificString(double number, int digits_before, int digits_after)
{
	//get the base 10 logarithm of the passed number and floor it
	int flooredlogten = (int)log10(number);

	//check if the number is can be written without scientific notation, given the allowed number of digits before the period
	if (digits_before < flooredlogten)
	{
		std::ostringstream strs;
		strs.precision(digits_after);
		strs << scientific << number;
		return strs.str();
	}
	else
	{
		//otherwise, just round the number to the requested amount of digits after the period and return it
		return doubleToString(number, digits_after);
	}
}

string Helpers::doubleToUnitString(double number, string unit)
{
	string retstring;
	if (number >= 10e9)
	{
		//larger than 10 billion, scale to Giga
		retstring = doubleToString(number / 1e9, 2);
		retstring = retstring + " G" + unit;
	}
	else if (number >= 10e6)
	{
		//larger than 10 million, scale to Mega
		retstring = doubleToString(number / 1e6, 2);
		retstring = retstring + " M" + unit;
	}
	else if (number >= 10e3)
	{
		//larger than 10000, scale to kilo
		retstring = doubleToString(number / 1e3, 2);
		retstring = retstring + " k" + unit;
	}
	else if (number < 0.01)
	{
		//smaller than 0.01, scale to mili
		retstring = doubleToString(number * 1e3, 2);
		retstring = retstring + " m" + unit;
	}
	else
	{
		//between 0.01 and 10000, just round to two digits and add the unit
		retstring = doubleToString(number, 2) + " " + unit;
	}
	return retstring;
}

string Helpers::intToString(int number)
{
	std::ostringstream strs;
	strs << number;
	return strs.str();
}

void Helpers::writeToLog(std::string &logMsg, LOGLEVEL priority, bool clear)
//writes a log message. clear doesn't do anything but was kept for stackeditor compatibility
{
	if (priority >= loglevel)
	{
		string logstring = "IMS2:[";
		switch (priority)
		{
		case L_ERROR:
			logstring += "!!ERROR!!] ";
			break;
		case L_WARNING:
			logstring += "!WARNING!] ";
			break;
		case L_MESSAGE:
			logstring += "MESSAGE] ";
			break;
		case L_DEBUG:
			logstring += "DEBUG] ";
			break;
		}
		logstring += logMsg;
		oapiWriteLog((char*)logstring.c_str());
	}
}

void Helpers::SetLogLevel(LOGLEVEL minimumpriority)
{
	loglevel = minimumpriority;
}

UINT Helpers::GetNewUID()
{
	static UINT uid = 0;
	uid++;
	return uid;
}


VECTOR3 Helpers::parseVector3Parameter(vector<string> line)
{
	if (line.size() < 4)
	{
		throw invalid_argument("Error: invalid vector definition for \"" + line[0] + "\"");
	}
	return _V(atof(line[1].data()),
		atof(line[2].data()), atof(line[3].data()));
}


void Helpers::stringToLower(string &str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}

double Helpers::fixDoubleNaN(double input)
{
	if (input != input)
	{
		return 0.0;
	}
	return input;
}

