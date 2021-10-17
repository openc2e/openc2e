// CreatureMoniker.cpp

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "CreatureMoniker.h"
#include <vector>
#include <string>
#include "../../common/GameInterface.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////

CCreatureMoniker::CCreatureMoniker() : type (SELECTED_CREATURE), uniqueID(0)
{
}

CCreatureMoniker::CCreatureMoniker(TARG_TYPE ltype, int luniqueID, std::string name)
 : type (ltype), uniqueID(luniqueID), myName(name)
{
}

bool CCreatureMoniker::operator==(const CCreatureMoniker& other)
{
	return (type == other.type) && (uniqueID == other.uniqueID);
}

// Bit of CAOS script used to target this moniker
CString CCreatureMoniker::TargCAOS()
{
	if (type == RANDOM_TARGET)
		return _T("loop rtar 0 0 0 untl type targ = 7\n");
	else if (type == SELECTED_CREATURE)
		return _T("targ norn\n");
	else if (type == FIXED_UNID)
	{
		CString target;
		target.Format(_T("targ agnt %d\n"), uniqueID);
		return target;
	}

	ASSERT(false);
	return _T("\n"); // engine will throw invalid targ exception 
}

std::ofstream& operator<<(std::ofstream& out, const CCreatureMoniker& monik)
{
	out << (int)monik.type << std::endl;
	out << monik.uniqueID << std::endl;
	if (monik.myName.empty())
		out << "unnamed" << std::endl;
	else
		out << monik.myName << std::endl;
	
	return out;
}

std::ifstream& operator>>(std::ifstream& in, CCreatureMoniker& monik)
{
	int type;
	in >> type;
	monik.type = (CCreatureMoniker::TARG_TYPE)type;
	in >> monik.uniqueID;
	in >> std::ws;
	std::getline(in, monik.myName);

	return in;
}


//////////////////////////////////////////////////////////////////////

CCreatureNames::CCreatureNames()
{
	moniks.push_back(CCreatureMoniker(CCreatureMoniker::SELECTED_CREATURE, 0));
	names.push_back(_T("Selected creature"));

	// Dynamically quiz game for current set of creatures
	std::string command = "enum 0 0 0 doif type targ = 7 outv unid outs \"\\n\" outs hist name gtos 0 outs \"\\n\" outv gnus outs \"\\n\" outv spcs outs \"\\n\" endi next";
	std::string reply;

	bool bOK = theGameInterface.Inject(command.c_str(), reply);
	if (bOK)
	{
		std::istringstream strin(reply);
		while (strin.good())
		{
			std::string unidStr, name, genusStr, speciesStr;
			std::getline(strin, unidStr);
			std::getline(strin, name);
			std::getline(strin, genusStr);
			std::getline(strin, speciesStr);
			if (!strin.good())
				break;

			if (name.empty())
				name = "Unnamed";
			name += " - ";

			int gender = atoi(speciesStr.c_str());
			if (gender == 1)
				name += "Male";
			else if (gender == 2)
				name += "Female";
			else 
				name += "Genderless";

			int genus = atoi(genusStr.c_str());
			name += " ";
			if (genus == 1)
				name += "Norn";
			else if (genus == 2)
				name += "Grendel";
			else if (genus == 3)
				name += "Ettin";
			else if (genus == 4)
				name += "Geat";
			else 
				name = "Unknown";

			int unid = atoi(unidStr.c_str());
			moniks.push_back(CCreatureMoniker(CCreatureMoniker::FIXED_UNID, unid, name));
			names.push_back(name.c_str());
		}
	}

	moniks.push_back(CCreatureMoniker(CCreatureMoniker::RANDOM_TARGET, 0));
	names.push_back(_T("Random target"));
}

CString CCreatureNames::GetCreatureName(int i)
{
	ASSERT(i >= 0);
	ASSERT(i < names.size());
	ASSERT(names.size() == moniks.size());
	return names[i];
}

CCreatureMoniker CCreatureNames::GetMoniker(int i)
{
	ASSERT(i >= 0);
	ASSERT(i < moniks.size());
	ASSERT(moniks.size() == names.size());
	return moniks[i];
}

int CCreatureNames::size()
{
	return names.size();
}

// static
CCreatureNames* CCreatureNames::SnapshotCreatureNames()
{
	return new CCreatureNames();
}

// static
CString CCreatureNames::MonikerToName(const CCreatureMoniker& monik)
{
	if (monik.type == CCreatureMoniker::RANDOM_TARGET)
		return _T("(Random)");
	else if (monik.type == CCreatureMoniker::SELECTED_CREATURE)
		return _T("(Selected)");
	else if (monik.type == CCreatureMoniker::FIXED_UNID)
		return CString(_T("(")) + CString(monik.myName.c_str()) + _T(")");
	else
	{
		ASSERT(false);
		return _T("(Unknown)");
	}
}

