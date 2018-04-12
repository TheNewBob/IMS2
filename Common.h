//module type list in this file must be modified if new module types are added!!

#pragma once

#include "orbitersdk.h"
#include "WinDef.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include "Helpers.h"
#include "Moduletypes.h"
#include "Exceptions.h"
#include "assert.h"
#include "Olog.h"
#include "OpForwardDeclare.h"

//forward declarations
class IMS2;
class IMS_Module;
class IMS_PressurisedModule;
struct DOCKEDVESSEL;

#include "IMSATTACHEMNTPOINT.h"

using namespace std;


static double RAD_PER_DEGREE = 0.0174532925;

typedef enum
{
	MESSAGE,
	WARNING,
	IMMINENT_FAILURE,
	CATASTROPHY
} SYSTEMWARNINGPRIORITY;

//vessel types
typedef enum
{
	DEF_VESSEL,
	IMS_VESSEL
}	VESSELTYPE;

struct IMSATTACHMENTPOINT;
//struct for storing properties of an attachment point
/*struct IMSATTACHMENTPOINT
{
	VECTOR3 pos;
	VECTOR3 rot;
	VECTOR3 dir;
	double rotModifier;
	DOCKHANDLE dockPort;
	std::string id;
	bool exists;
	IMSATTACHMENTPOINT *connectsTo;
	DOCKEDVESSEL *dockedVessel;
	IMS_Module *isMemberOf;
};*/


//struct for simplifying managing docked vessels
struct DOCKEDVESSEL
{
	VESSEL *vessel;
	IMSATTACHMENTPOINT *attachedTo;
	DOCKHANDLE dockPortOther;
	VESSELTYPE type;
};

/**
* \brief A thruster mode describes all propperties needed by a thruster.
*
* Thrusters can have several modes if appropriate (most famously the VASIMR)
*/
struct THRUSTERMODE
{
	string Name = "";
	double Thrust = -1;
	double Isp = -1;
	double Efficiency = 1.0;
	vector<int> Propellants;
	vector<float> Ratio;
	double Exhaustlength = 1.0;
	double Exhaustwidth = 1.0;
};

/**
* \brief Defines a thruster exhaust texture in orbiter
*/
struct THRUSTEREXHAUST
{
	THRUSTEREXHAUST(){};
	THRUSTEREXHAUST(const THRUSTEREXHAUST &copy)
	{
		length = copy.length;
		width = copy.width;
		pos = copy.pos;
		dir = copy.dir;
	};
	double length = -1;
	double width = -1;
	VECTOR3 pos = _V(0, 0, 0);
	VECTOR3 dir = _V(0, 0, 0);
};


//oh the joy of function objects...
//this is only used to speed up lookup times when checking if a certain VESSEL is docked.
class DockedVesselToVesselComparator
{
public:
	DockedVesselToVesselComparator(VESSEL *_vessel) : vessel(_vessel){};
	bool operator() (DOCKEDVESSEL *dv)
	{
		return dv->vessel == vessel;
	}
private:
	VESSEL *vessel;
};