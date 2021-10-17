// CreatureMoniker.h

#ifndef CREATURE_MONIKER_H
#define CREATURE_MONIKER_H

#pragma once

#include <vector>
#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////

class CCreatureNames;

class CCreatureMoniker
{
public:
	typedef enum
	{
		RANDOM_TARGET,
		SELECTED_CREATURE,
		FIXED_UNID
		
	} TARG_TYPE;

	TARG_TYPE type;
	int uniqueID;
	std::string myName;

public:
	CCreatureMoniker();
	CCreatureMoniker(TARG_TYPE ltype, int luniqueID, std::string name = "");
	bool operator==(const CCreatureMoniker& other);
	CString CCreatureMoniker::TargCAOS();
};

std::ofstream& operator<<(std::ofstream& out, const CCreatureMoniker& monik);
std::ifstream& operator>>(std::ifstream& in, CCreatureMoniker& monik);

//////////////////////////////////////////////////////////////////////

class CCreatureNames  
{
private:
	std::vector<CString> names;
	std::vector<CCreatureMoniker> moniks;

public:
	CCreatureNames();
	CString GetCreatureName(int i);
	CCreatureMoniker GetMoniker(int i);
	int size();

	// should return an auto_ptr, but MFC STL doesn't have it in (growl)
	static CCreatureNames* SnapshotCreatureNames();
	static CString MonikerToName(const CCreatureMoniker& monik);
};

//////////////////////////////////////////////////////////////////////

#endif // CREATURE_MONIKER_H

