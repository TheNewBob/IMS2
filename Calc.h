#pragma once

//class to group various static functions having to do with numbers and calculations

class Calc
{
public:
	Calc();
	~Calc();

	static bool IsNear(const double number, const double compare, const double inaccuracy);
	static bool IsNear(const VECTOR3& v1, const VECTOR3& v2, double inaccuracy);
	static bool IsEqual(double n1, double n2);
	static bool IsEqual(const VECTOR3& v1, const VECTOR3& v2);
	static double DirectionSimilarity(const VECTOR3& v1, const VECTOR3& v2);
	static bool IsDirectionSimilar(const VECTOR3& v1, const VECTOR3& v2, double threshold);

	/**
	 * \brief Rounds a double to the next integer
	 */
	static int Round(double d);
	
	/**
	 * \brief Rounds a double to a certian precision
	 * \param d The number to be rounded
	 * \param precision d will be rounded to the digit conforming to 1 / precision (e.g. precision 1000 rounds to the third digit behind the comma).
	 *	This should always be divisible by 10!
	 */
	static double Round(double d, double precision);
	static void RoundVector(VECTOR3& v, double precision);

};

