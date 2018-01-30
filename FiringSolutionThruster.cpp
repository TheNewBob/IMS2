#include "common.h"
#include "FiringSolutionThruster.h"
#include "Calc.h"


FiringSolutionThruster::FiringSolutionThruster(THRUSTER_HANDLE thruster, VESSEL *vessel)
	: thruster(thruster)
{
	//get the position, direction and thrust from orbiter
	vessel->GetThrusterRef(thruster, pos);
	vessel->GetThrusterDir(thruster, dir);
	maxthrust = vessel->GetThrusterMax0(thruster);

	initForces();
}


FiringSolutionThruster::FiringSolutionThruster(VECTOR3 pos, VECTOR3 dir, double maxthrust)
	: pos(pos), dir(dir), maxthrust(maxthrust), thruster(NULL)
{
	initForces();
}


FiringSolutionThruster::~FiringSolutionThruster()
{
}


void FiringSolutionThruster::initForces()
{
	//round the position and direction to 5 digits to avoid precision artifacts
	Calc::RoundVector(dir, 100000);
	Calc::RoundVector(pos, 100000);

	//calculate the thrusters torque and linear force vectors
	linearforce = dir * maxthrust;
	//calculate the torque efficiency. thrust is not yet considered at this point.
	//We're inverting the result to convert to orbiters lefthandedness.
	torque = crossp(pos, dir) * -1;
	
	//round force and torque to mN / mNm, anything less gets insignifficant really fast.
	setTranslationGroups();
	setRotationGroups();

	//after doing the scoring for the groups, calculate the actual torque taking thrust into consideration
	torque *= maxthrust;
	
	//round torque and force to mNm / mN
	Calc::RoundVector(torque, 1000);
	Calc::RoundVector(linearforce, 1000);

}


void FiringSolutionThruster::setTranslationGroups()
{
	//the directional vector is normalised, meaning that a thruster pointing in 45 degrees 
	//in 2 axes has a magnitude of 0.57735 in every axis (D^2 = A^2 + B^2 + C^2). 
	//We consider this a threshold below which a thruster becomes ineffective to provide thrust 
	//on a certain axis.
	double threshold = 0.57735;

	//get the magnitudes of the vector axes
	double xmag = abs(dir.x);
	double ymag = abs(dir.y);
	double zmag = abs(dir.z);

	//decide which thrustergroups the thruster is suited for, and how well.
	//depending on its alignement, a thruster might be suited for multiple translation groups, although never opposite ones!
	if (xmag >= threshold)
	{
		//this thruster could potentially supply thrust along the x axis (left/right)
		if (dir.x > 0)
		{
			groupsuitability[THGROUP_ATT_RIGHT] = xmag;
		}
		else
		{
			groupsuitability[THGROUP_ATT_LEFT] = xmag;
		}
	}
	if (ymag >= threshold)
	{
		//this thruster could potentially supply thrust along the y axis (up/down)
		if (dir.y > 0)
		{
			groupsuitability[THGROUP_ATT_UP] = ymag;
		}
		else
		{
			groupsuitability[THGROUP_ATT_DOWN] = ymag;
		}
	}
	if (zmag >= threshold)
	{
		//this thruster could potentially supply thrust along the z axis (forward/backward)
		if (dir.z > 0)
		{
			groupsuitability[THGROUP_ATT_FORWARD] = zmag;
		}
		else
		{
			groupsuitability[THGROUP_ATT_BACK] = zmag;
		}
	}
}


void FiringSolutionThruster::setRotationGroups()
{
	//We won't do premature selection for the rotation groups. For now, ta thruster is assigned to any group it can provide some little torque for.

	//get the magnitudes of the vector axes
	double xmag = abs(torque.x);
	double ymag = abs(torque.y);
	double zmag = abs(torque.z);
	double threshold = 0.1;

	//get the actual torque efficiency
	double xefficiency = abs(torque.x);
	double yefficiency = abs(torque.y);
	double zefficiency = abs(torque.z);

	//decide which thrustergroups the thruster is suited for, and how well.
	//depending on its position and alignement, a thruster might be suited for multiple rotation groups, although never opposite ones!
	//The score symbolises the thrusters absolute efficiency at producing torque, not just which one it does best.
	if (xmag >= threshold)
	{
		//this thruster could potentially supply torque around the x axis (pitch)
		if (torque.x < 0)
		{
			groupsuitability[THGROUP_ATT_PITCHDOWN] = xefficiency;
		}
		else
		{
			groupsuitability[THGROUP_ATT_PITCHUP] = xefficiency;
		}
	}
	if (ymag >= threshold)
	{
		//this thruster could potentially supply torque around the y axis (yaw)
		if (torque.y < 0)
		{
			groupsuitability[THGROUP_ATT_YAWRIGHT] = yefficiency;
		}
		else
		{
			groupsuitability[THGROUP_ATT_YAWLEFT] = yefficiency;
		}
	}
	if (zmag >= threshold)
	{
		//this thruster could potentially supply torque around the z axis (bank/roll)
		if (torque.z < 0)
		{
			groupsuitability[THGROUP_ATT_BANKLEFT] = zefficiency;
		}
		else
		{
			groupsuitability[THGROUP_ATT_BANKRIGHT] = zefficiency;
		}
	}
}


double FiringSolutionThruster::GetScore(THGROUP_TYPE group)
{
	Helpers::assertThat([this]() { return groupsuitability.size() > 0; }, "Thruster was found unsuitable for any thruster group!");
	auto grp = groupsuitability.find(group);
	if (grp == groupsuitability.end())
	{
		return 0;
	}
	return grp->second;
}


bool FiringSolutionThruster::AreSymmetricTwins(THGROUP_TYPE group, FiringSolutionThruster *a, FiringSolutionThruster *b)
{
	return false;
}






