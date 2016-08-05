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

	static int Round(double d);
	static void RoundVector(VECTOR3& v, double precision);

};

