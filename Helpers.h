#pragma once

using namespace std;

//wrapper for orbiter FILEHANDLE to ensure compatibility with StackEditor
struct IMSFILE
{
	FILEHANDLE file;

	IMSFILE(FILEHANDLE f)
	{
		file = f;
	}
};


//class with static functions, mostly for string operations
class Helpers
{
public:
	static void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
	
	/* puts a tokenized string back together, separating tokens by " "
	*/
	static string WriteTokenizedString(vector<string> &tokens);

	static void resetFile(IMSFILE file);
	
	/**
	 * \brief sets the minimum priority that a logmessage needs to have to get loged
	 * \param minimumpriority Only messages equal or higher than the priority will be loged
	 */
	static UINT GetNewUID();

	//functions written by Meson800 for stackeditor
	static bool readLine(IMSFILE file, std::vector<std::string>& tokens, const std::string &delimiters);
	
	/**
	 * \brief removes leading and trailing whitespace
	 * \param str Reference to the string you want to remove whitespace
	 */
	static void removeExtraSpaces(std::string& str);
	static int stringToInt(const std::string& inputString);
	static double stringToDouble(const std::string& inputString);
	
	/**
	 * \brief Converts a double to a string
	 * \param number The number to be converted
	 * \param digits Number of significant digits behind the period to print. Pass -1 to print the entire double
	 * \return a string representing the number with the requested amount of digits behind the coma
	 */
	static string doubleToString(double number, int digits = -1);
	
	/**
	 * \brief Converts a double into a string in scientific notation
	 * \param number The number to be converted to a string
	 * \param digits_before Number of digits allowed before the period. Acts as a threshold below which numbers are written without scientific notation.
	 * \param digits_after Number of digits to render after the period.
	 */
	static string doubleToScientificString(double number, int digits_before, int digits_after);

	/**
	 * \brief Takes a number and a unit, e.g. 100254, "m" and returns a string looking like e.g. "100.25 Km".
	 * \param number The number to scale
	 * \param unit The unit symbol to append to the representation
	 * \return A string that might look like this: 2543.32 Mm
	 * \note Scales units if the number needs more than 5 digits before the period to represent.
	 *	Always leaves 2 digits behind the comma.
	 *	scales from m(ili) to G(iga).
	 * \note Does not work for units that are already scaled (don't pass km for unit, a mkm would be a rather ridiculous 
	 *	designation for meters) or have special symbols (kg and t, most obviously).
	 */
	static string doubleToUnitString(double number, string unit);

	static string intToString(int number);
	static bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }
	static const string whitespace;																//!< contains space and tab for space removal operations
	static void stringToLower(string &str);
	
	/* \brief parses and returns a VECTOR3 from an already tokenised line
	 * line: a vector of strings where the first item is the parameter name, 
	 * followed by x y and z values for the vector
	 */
	static VECTOR3 parseVector3Parameter(vector<string> line);

	/**
	 * \return 0 if the argument is NaN, the argument itself otherwise
	 * \param input A double you want to make sure isn't NaN
	 */
	static double fixDoubleNaN(double input);
	
};
