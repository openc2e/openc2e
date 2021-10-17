// DialogSpec.h: interface for the CDialogSpec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DIALOG_SPEC_H
#define DIALOG_SPEC_H

#include <vector>
#include <iostream>
#include <fstream>

#include "CreatureMoniker.h"

#pragma once

class CDialogSpec
{
public:
	typedef enum
	{
		GRAPH_DIALOG,
		LOG_DIALOG,
		INJECTION_DIALOG,
		ORGAN_DIALOG
	} GRAPH_TYPE;

	// General
	std::string name;
	GRAPH_TYPE type;
	std::vector<int> vChems;
	int nSampleRate; // milliseconds
	CCreatureMoniker monik;
	
	// Graph only
	int nHistorySize;  // number of nSampleRates that we store
	CRect rectPosition; // measured in dialog units
	bool bDrawKey;
	bool bWhiteColour;

	// Log file only
	std::string filename;

public:
	CDialogSpec();
	CString MakeName() const;
	CString TypeString() const;
};

std::ofstream& operator<<(std::ofstream& out, CDialogSpec& spec);
std::ifstream& operator>>(std::ifstream& in, CDialogSpec& spec);

#endif // DIALOG_SPEC_H

