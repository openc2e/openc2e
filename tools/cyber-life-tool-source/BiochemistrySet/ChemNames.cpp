// ChemNames.cpp: implementation of the CChemNames class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "ChemNames.h"
#include "../../common/Catalogue.h"
#include "../../common/WhichEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// the one and only instantiation
CChemNames theChemName;

CChemNames::Init()
{
	theChemName.InitObject();
}

CChemNames::InitObject()
{
	// our catalogue handling class to get chemical names
	Catalogue theCatalogue;
	try
	{
		std::string catpath = theWhichEngine.CatalogueDir();
		theCatalogue.AddDir( catpath, "en" ); // default to english (ugh)
		ASSERT(size() == 256); // catalogue only has 255 chems in it

		names.resize(size());
		for (int i = 0; i < size(); ++i)
		{
			names[i] = theCatalogue.Get("chemical_names", i);
		}
	}
	catch( Catalogue::Err& e )
	{
		MessageBox(NULL, _T("Problem getting chemical names\n\n") + CString(e.what()), _T("Biochemistry Set"), MB_OK | MB_ICONERROR);
		exit(0);
	}
}

// static
CString CChemNames::GetChemName(int i)
{
	ASSERT(i >= 0);
	ASSERT(i < size());
	ASSERT(theChemName.names.size() == size());
	return theChemName.names[i];
}

// static
CString CChemNames::GetChemNameAndNumber(int i)
{
	ASSERT(i >= 0);
	ASSERT(i < size());
	ASSERT(theChemName.names.size() == size());


	CString sChemNum;
	sChemNum.Format(_T("%d"), i);
	if (theChemName.names[i] == sChemNum)
		return theChemName.names[i];
	else
		return theChemName.names[i] + " (" + sChemNum + ")";
}

