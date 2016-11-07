#include "Common.h"
#include "FiringSolutionThruster.h"
#include "FSThrusterCollection.h"

const double FSThrusterCollection::MINRELATIVEEFFICIENCY = 0.3;


FSThrusterCollection::FSThrusterCollection(vector<THRUSTER_HANDLE> thrusters, VESSEL *vessel)
{
	thrusterlist.reserve(thrusters.size());
	//create the FiringSollutionTrhusters from the passed handles
	//read out thruster properties and assign them to groups they seem well suited for
	for (UINT i = 0; i < thrusters.size(); ++i)
	{
		thrusterlist.push_back(new FiringSolutionThruster(thrusters[i], vessel));
	}
	createGroupLists();
}

FSThrusterCollection::FSThrusterCollection(vector<FiringSolutionThruster> &thrusters)
{
	deletethrustersonexit = false;
	thrusterlist.reserve(thrusters.size());

	for (UINT i = 0; i < thrusters.size(); ++i)
	{
		thrusterlist.push_back(&thrusters[i]);
	}
	createGroupLists();
}


FSThrusterCollection::~FSThrusterCollection()
{
	if (deletethrustersonexit)
	{
		for (UINT i = 0; i < thrusterlist.size(); ++i)
		{
			delete thrusterlist[i];
		}
	}
}

vector<FiringSolutionThruster*> FSThrusterCollection::GetGroupUnion(vector<THGROUP_TYPE> &groups, THGROUP_TYPE governing_group)
{
	assert(groups.size() > 0 && "Passed an empty groups list to calculate intersection!");

	vector<FiringSolutionThruster*> result;
	//We only need to check the thrusters in the governing group
	vector<FiringSolutionThruster*> &possiblethrusters = grouplists[governing_group];
	for (UINT i = 0; i < possiblethrusters.size(); ++i)
	{
		//check if the thruster is also a member of any of the other groups
		for (UINT j = 0; j < groups.size(); ++j)
		{
			auto it = possiblethrusters[i]->groupsuitability.find(groups[j]);
			if (it != possiblethrusters[i]->groupsuitability.end())
			{
				result.push_back(possiblethrusters[i]);
				break;			//no need to look further. We certainly don't want it twice!
			}
		}
	}

	//sort the return vector by the group scores of the desired group
	sort(result.begin(), result.end(), SortByGroupScore(governing_group));
	return result;
}

map<FORCETYPE, VECTOR3> FSThrusterCollection::GetTotalForces(THGROUP_TYPE group)
{
	return totalforces[group];
}


void FSThrusterCollection::createGroupLists()
{
	//compare the scores of all thrusters to maximum scores in the groups they are assigned to.
	//thrusters that come out too inefficient relative to the top-scorers are culled from the group.
	compareThrusterScores();

	//set the final ranking of thrusters within their groups
	applyFinalScores();

	//create a list of thrusters for every group, sorted descendingly by their score for that group.
	//map the thrusters to the groups they're assigned to
	for (UINT i = 0; i < thrusterlist.size(); ++i)
	{
		map<THGROUP_TYPE, double> groups = thrusterlist[i]->GetGroups();
		for (auto j = groups.begin(); j != groups.end(); ++j)
		{
			grouplists[j->first].push_back(thrusterlist[i]);
		}
	}
	//now sort the thrusters in their groups by score
	for (auto i = grouplists.begin(); i != grouplists.end(); ++i)
	{
		sort(i->second.begin(), i->second.end(), SortByGroupScore(i->first));
	}
}


void FSThrusterCollection::compareThrusterScores()
{
	//get the maximum scores for each group
	map<THGROUP_TYPE, double> maxscores = getMaximumGroupScores();

	for (UINT i = 0; i < thrusterlist.size(); ++i)
	{
		map<THGROUP_TYPE, double> curgrps = thrusterlist[i]->groupsuitability;
		//for every thuster, compare its group scores to the maximum scores
		for (auto j = maxscores.begin(); j != maxscores.end(); ++j)
		{
			auto curgroup = curgrps.find(j->first);
			//is the thruster in the currently compared group?
			if (curgroup != curgrps.end())
			{
				//remove the thruster from that group if its score is too far below the maximum score
				if (curgroup->second < j->second * MINRELATIVEEFFICIENCY)
				{
					curgrps.erase(j->first);
				}
			}
		}
		if (curgrps.size() > 2)
		{
			//we have at least 3 well suited thrusters left, that'll do.
			//otherwise, we won't apply the changes, because ill-suited thrusters are still better than no thrusters.
			thrusterlist[i]->groupsuitability = curgrps;
		}
	}
}


void FSThrusterCollection::applyFinalScores()
{
	for (UINT i = 0; i < thrusterlist.size(); ++i)
	{
		auto &curgrps = thrusterlist[i]->groupsuitability;
		//divide the group score of every thruster by the amount of groups it is assigned to.
		for (auto j = curgrps.begin(); j != curgrps.end(); j++)
		{
			j->second /= curgrps.size();
		}
	}
}


map<THGROUP_TYPE, double> FSThrusterCollection::getMaximumGroupScores()
{
	map<THGROUP_TYPE, double> maxscores;
	for (UINT i = 0; i < thrusterlist.size(); ++i)
	{
		map<THGROUP_TYPE, double> &curgrps = thrusterlist[i]->groupsuitability;
		for (auto j = curgrps.begin(); j != curgrps.end(); ++j)
		{
			if (j->second > maxscores[j->first])
			{
				maxscores[j->first] = j->second;
			}
		}
	}
	return maxscores;
}


void FSThrusterCollection::calculateOverallForces()
{
	//walk through all groups
	for (auto i = grouplists.begin(); i != grouplists.end(); ++i)
	{
		VECTOR3 force, torque;
		//walk through all thrusters in the group and add their vectors
		for (UINT j = 0; j < i->second.size(); ++j)
		{
			force += i->second[j]->linearforce;
			torque += i->second[j]->torque;
		}
		//note the resulting vectors in a map
		(totalforces[i->first])[F_LINEAR] = force;
		(totalforces[i->first])[F_TORQUE] = torque;
	}
}

/*bool FSThrusterCollection::SortByGroupScore(FiringSolutionThruster *a, FiringSolutionThruster *b)
{
	if (a->GetScore(sortbygroup) == b->GetScore(sortbygroup))
	{
		//in case the scores are equal, sort by memory address to guarantee transitivity 
		//(i.e. the result of the operation does not depend on the order in which the elements are passed,
		//because std::sort does not like that).
		return a > b;
	}
	//if they are not equal, the group score is of course deciding

}*/

