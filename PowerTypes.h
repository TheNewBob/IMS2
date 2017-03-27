#pragma once

/**
 * \file This file contains various types neccessary for all power-related classes to work.
 */

const double MILIS_PER_HOUR = 3600 * 1000;			//!< number of miliseconds in an hour.

/**
* Contains the maximum, minimum and current voltage of a parent/child.
*/
struct VOLTAGE_INFO
{
	double minimum = -1;
	double maximum = -1;
	double current = -1;

	/**
	* \return true if the two voltage ranges have overlap.
	* \param othervoltage Another voltage range to compare to.
	*/
	bool IsRangeCompatibleWith(VOLTAGE_INFO othervoltage)
	{
		if (this->maximum < othervoltage.minimum ||
			this->minimum > othervoltage.maximum)
		{
			return false;
		}
		return true;
	}
};

enum POWERPARENT_TYPE
{
	PPT_BUS,
	PPT_SOURCE,
	PPT_CONVERTER
};

enum POWERCHILD_TYPE
{
	PCT_CONSUMER,
	PCT_BUS,
	PCT_CONVERTER
};
