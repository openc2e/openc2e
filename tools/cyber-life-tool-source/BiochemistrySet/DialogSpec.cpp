// DialogSpec.cpp: implementation of the CDialogSpec class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "DialogSpec.h"

#include <string>
#include <istream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogSpec::CDialogSpec()
	: rectPosition(0, 0, 350, 200), name(_T("")), filename(_T("biochem.csv"))
{
	type = GRAPH_DIALOG;
	nSampleRate = 200;
	nHistorySize = 150;
	bDrawKey = true;
	bWhiteColour = false;
}


std::ofstream& operator<<(std::ofstream& out, CDialogSpec& spec)
{
	// can't serialise empty strings as the serialise
	// in sucks up consecutive carriage returns
	if (spec.name.empty())
	{
		ASSERT(false); // should be picked up elsewhere
		spec.name = _T("<unnamed>");
	}
	out << spec.name << std::endl;
	out << (int)spec.type << std::endl;
	
	int n = spec.vChems.size();
	out << n << std::endl;
	for (int i = 0; i < n; ++i)
	{
		out << spec.vChems[i] << std::endl; 
	}
	out << spec.nSampleRate << std::endl; 
	out << spec.monik;

	out << spec.nHistorySize << std::endl;  
	out << spec.rectPosition.top << std::endl << spec.rectPosition.left << std::endl << spec.rectPosition.bottom << std::endl << spec.rectPosition.right << std::endl;
	out << spec.bDrawKey << std::endl;
	out << spec.bWhiteColour << std::endl;

	if (spec.filename.empty())
		spec.filename = _T("biochem.csv");
	out << spec.filename << std::endl;

	return out;
}

std::ifstream& operator>>(std::ifstream& in, CDialogSpec& spec)
{
	in >> std::ws;
	std::getline(in, spec.name);

	ASSERT(!in.eof());
	ASSERT(!in.bad());
	ASSERT(!in.fail());
	ASSERT(in.is_open());

	int type;
	in >> type;
	spec.type = (CDialogSpec::GRAPH_TYPE)type;
	
	int n;
	in >> n;
	spec.vChems.resize(n);
	for (int i = 0; i < n; ++i)
	{
		in >> spec.vChems[i]; 
	}
	in >> spec.nSampleRate; 
	in >> spec.monik;

	in >> spec.nHistorySize;  
	in >> spec.rectPosition.top >> spec.rectPosition.left >> spec.rectPosition.bottom >> spec.rectPosition.right;
	in >> spec.bDrawKey;
	in >> spec.bWhiteColour;

	in >> std::ws;
	std::getline(in, spec.filename);

	return in;
}

CString CDialogSpec::MakeName() const
{
	CString name;
	name.Format(_T("%s %s %.2fs"), TypeString(), CCreatureNames::MonikerToName(monik), ((float)nSampleRate)/1000.0f);

	return name;
}

CString CDialogSpec::TypeString() const
{
	if (type == GRAPH_DIALOG)
		return _T("Graph");
	else if (type == LOG_DIALOG)
		return _T("Log");
	else if (type == INJECTION_DIALOG)
		return _T("Injection");
	else if (type == ORGAN_DIALOG)
		return _T("Organ");
	else
	{
		ASSERT(false);
		return 	_T("Unknown");
	}
}

