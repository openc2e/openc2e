// --------------------------------------------------------------------------------------
// File:		ScriptExecutive.cpp
// Class:		ScriptExecutive
// Purpose:		To parse and handle the NextGEN scripts and generate the permutations
// Description:	
//  Whilst the heavy work is done elsewhere, this is the heavyweight class which hubs
//  the work of the program.
//
// History:
//	16Jun99		DanS	Initial Version
//
// --------------------------------------------------------------------------------------

#ifndef SCRIPTEXECUTIVE_H
#define SCRIPTEXECUTIVE_H

#include <string>
#include <vector>

#include "Source.h"
#include "Genetics.h"


extern bool theReplaceByRandomDataFlag;

extern bool theRandomlyReorderFlag;



namespace NextGEN
{

// --------------------------------------------------------------------------------------
// Method:		expandFileRef
// Arguments:	char* filname - The filename complete with aliases to expand.
// Returns:		std::string containing expanded reference
// Description:	Uses the alias expansion method to expand $n aliases in filename
// --------------------------------------------------------------------------------------

std::string expandFileRef(char* filename);

class ScriptExecutive
{
private:
	//Private states...
	// Attribute: mySourceName - the name of the script.
	std::string mySourceName;

	// Attribute: mySources - The list of sourceline objects representing the script
	std::vector<Source*> mySources;

	// Attribute: myResults - The resulting genetics objects
	std::vector<Genetics*> myResults;

	// Attribute: myDumpFolder - The Destination folder for the genetics.
	std::string myDumpFolder;

	// Attribute: myVerbosityLevel - The verbosity level of the script environment
	int myVerbosityLevel;

	// Attribute: myOutputMode - Which genus sets to write, n, g, e etc.
	int myOutputMode;

public:
	//Public stuff...

	// ----------------------------------------------------------------------------------
	// Constructor
	// Arguments:	char* filename - The filename of the script file.
	//				int verbosity  - The verbosity of the environment (requested by user)
	// Returns:		(None)
	// Description:	Parses the script, and readies it for execution.
	// ----------------------------------------------------------------------------------
	ScriptExecutive(char* filename, int verbosity);

	// ----------------------------------------------------------------------------------
	// Destructor
	// Arguments:	(None)
	// Returns:		(None)
	// Description:	Cleans the whole caboodle up.
	// ----------------------------------------------------------------------------------
	~ScriptExecutive();

	// ----------------------------------------------------------------------------------
	// Method:		Prepare
	// Arguments:	(None)
	// Returns:		True if preparation worked.
	// Description:	Prepares all the sourcelines, thus genetics etc.
	// ----------------------------------------------------------------------------------
	bool Prepare();

	// ----------------------------------------------------------------------------------
	// Method:		Execute
	// Arguments:	(None)
	// Returns:		(None)
	// Description:	Performs the permutation operations.
	// ----------------------------------------------------------------------------------
	void Execute();

};

}

#endif

