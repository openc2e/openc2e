// --------------------------------------------------------------------------------------
// File:		GnoFile.h
// Classes:		GNOEntry, GNOFile
// Purpose:		Storage and manipulation of the GNO representations throughout NextGEN
// Description:	
//  The GNOEntry structure holds information for SVNote and also standard gene notes.
//
//  The GNOFile class manages the GNOEntry structures for any particular disk based
//  .GEN file, allowing GNO entries to be linked into genes where needed.
//
// History:
//	16Jun99		DanS	Initial Version
//
// --------------------------------------------------------------------------------------

#ifndef GNOFILE_H
#define GNOFILE_H

#include "Genetics.h"
#include "defs.h"

#include <vector>
#include <string>

namespace NextGEN
{

struct GNOEntry
{
	byte		noteType;
	byte		type;
	byte		subtype;
	byte		id;
	byte		rule;
	std::string entry0;
	std::string entry1;
	std::string entry2;
	std::string entry3;
	std::string entry4;
	std::string entry5;
	std::string entry6;
	std::string entry7;
	std::string entry8;
	std::string entry9;
	std::string entry10;
	std::string entry11;
	std::string entry12;
	std::string entry13;
	std::string entry14;
	std::string entry15;
	std::string entry16;
};


class GNOFile
{
private:
	//Attribute: myGnoEntries - Stores the GNO entries associated with the file.
	std::vector<GNOEntry*> myGnoEntries;
public:
	// ----------------------------------------------------------------------------------
	// Constructor
	// Arguments:	char* filename	- The filename of the GNO file to load.
	// Returns:		(None)
	// Description:	Loads the GNO file into memory and generates the GNOEntry structures
	// ----------------------------------------------------------------------------------
	GNOFile(char* filename);

	// ----------------------------------------------------------------------------------
	// Destructor
	// Arguments:	(None)
	// Returns:		(None)
	// Description:	Cleans up the GNO entries associated with this GNOFile
	// ----------------------------------------------------------------------------------
	~GNOFile();

	// ----------------------------------------------------------------------------------
	// Method:		LinkGene
	// Arguments:	Gene* thisgene	- The gene to link all GNO entries who are associated
	// Returns:		(None)
	// Description:	Links all relevant GNO entries from this file to the gene passed.
	// ----------------------------------------------------------------------------------
	void LinkGene(Gene* thisgene);
};


}

#endif // GNOFILE_H

