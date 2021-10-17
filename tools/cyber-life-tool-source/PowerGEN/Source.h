// --------------------------------------------------------------------------------------
// File:		Source.h
// Class:		Source
// Purpose:		To parse and handle the source lines (* and +) in the NextGEN scripts
// Description:	
//  The Source class reads a sourceline and processes it, loading the genetics and GNO
//  files as needed, masking the genes as required, and performing variance cooercion
//  if the source command demands it.
//
// History:
//	16Jun99		DanS	Initial Version
//
// --------------------------------------------------------------------------------------

#ifndef SOURCE_H
#define SOURCE_H

#include "defs.h"
#include "GnoFile.h"
#include "Genetics.h"
#include "../../common/FileScanner.h"	

#include <vector>
#include <string>

namespace NextGEN
{

class Source
{
private:
	//Attribute: myInstruction - contains the instruction from the scriptfile
	std::string myInstruction;

	//Attribute: myVerbosity - the set verbosity level.
	int myVerbosity;

	//Attribute: myCooerce - Whether or not to cooerce the variance in the file
	bool myCooerce;

	//Attribute: myCooerceTo - The value to cooerce to.
	byte myCooerceTo;
public:

	//Attribute myGenomes - Stores the Genetics classes.
	std::vector<Genetics*> myGenomes;

	//Attribute myMerging - Whether we are merging to replacing.
	bool myMerging;

	//Attribute mySearchMasks - The masks associated with the genetics files.
	std::vector<Gene*> mySearchMasks;

	// ----------------------------------------------------------------------------------
	// Constructor
	// Arguments:	char* sourceline - The sourceline from the script.
	//				int verbosity	 - The verbosity of the execution environment.
	// Returns:		(None)
	// Description:	Processes the sourceline and readies the genetics for loading.
	// ----------------------------------------------------------------------------------
	Source(char* sourceline, int verbosity);

	// ----------------------------------------------------------------------------------
	// Destructor
	// Arguments, Returns:	(None)
	// Description:			Cleans up and releases memory
	// ----------------------------------------------------------------------------------
	~Source();
	
	// ----------------------------------------------------------------------------------
	// Method:		Prepare
	// Arguments:	(None)
	// Returns:		True on success
	// Description:	Loads the genetics files into memory and performs the trimmings etc.
	// ----------------------------------------------------------------------------------
	bool Prepare();
};

}

#endif

