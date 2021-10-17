// ChemNames.h: interface for the CChemNames class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CHEM_NAMES_H
#define CHEM_NAMES_H

#pragma once

#ifdef _MSC_VER
// Disable "debug symbol >255 chars" warning
#pragma warning( disable : 4786 )
#endif

#include <vector>
#include <string>

class CChemNames  
{
private:
	std::vector<CString> names;
	InitObject();

public:
	static Init();
	static CString GetChemName(int i);
	static CString GetChemNameAndNumber(int i);
	static int size()
	{
		return 256; // constant chemical list size for now
	}
};

#endif // CHEM_NAMES_H

