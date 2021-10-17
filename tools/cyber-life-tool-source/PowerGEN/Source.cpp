// --------------------------------------------------------------------------------------
// File:		Source.cpp
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

#include "Source.h"
#include "../../common/FileScanner.h"
#include "Genetics.h"
#include "GnoFile.h"
#include "defs.h"
#include "ScriptExecutive.h"

#include <vector>
#include <string>
#include <stdio.h>

namespace NextGEN
{

// ----------------------------------------------------------------------------------
// Constructor
// Arguments:	char* sourceline - The sourceline from the script.
//				int verbosity	 - The verbosity of the execution environment.
// Returns:		(None)
// Description:	Processes the sourceline and readies the genetics for loading.
// ----------------------------------------------------------------------------------
Source::Source(char *sourceline, int verbosity)
{
	std::string searchcriteria;
	//And here we have to parse the source line (either a + or a * for now)
	myVerbosity = verbosity;
	myInstruction.assign(sourceline);
	myMerging = (myInstruction.at(0) == '+');
	searchcriteria.assign(myInstruction.substr(1,myInstruction.find_first_of(",")-1));
	sourceline += (2+searchcriteria.length());

	std::string checkme;
	checkme.assign(sourceline);
	myCooerce = false;
	if (checkme.at(checkme.length()-2) == '!')
	{
		//we have to cooerce variant zero into the variant specified.
		myCooerce = true;
		myCooerceTo = ((checkme.at(checkme.length()-1))-'0');
		sourceline[checkme.length()-2] = 0;
	}

	if (verbosity > 6)
		printf("Search criteria parsed to: %s\n",searchcriteria.c_str());

	//Now sourceline points at the filespecs.
	//Each filespec should be separated by a space.

	char *startpos = sourceline;

	for(;*(sourceline-1) != 0;sourceline++)
	{
		if (*sourceline == 32 || *sourceline == 0)
		{
			bool incit = (*sourceline == 32);
			*sourceline = 0;
			std::string filespec = expandFileRef(startpos);
			if (verbosity > 4)
				printf("Looking for GEN files in %s\n",filespec.c_str());
			DirectoryScanner *scan = new DirectoryScanner(filespec.c_str());
			const char *gfilename = scan->getNextFile();
			if (gfilename != NULL)
			do
			{
				myGenomes.push_back(new Genetics(gfilename ,verbosity));
			} while ((gfilename = scan->getNextFile()) != NULL);
			delete scan;
			if (incit)
			{
				startpos = ++sourceline;
			}
		}
	}

	//next we run through the searchcriteria looking to build up CGene masks for each
	//of the criteria records.


	//FORMAT CURRENTLY is num num num num num num (all dec)

	Gene* gg;
	gg = (Gene*)malloc(sizeof(Gene));
	gg->gnoEntries = new std::vector<GNOEntry*>;
	sscanf(searchcriteria.c_str(),"%d %d %d %d %d %d",
		&(gg->variant),&(gg->group),&(gg->master),&(gg->gene),&(gg->subgroup),&(gg->subgene));

	mySearchMasks.push_back(gg);

	//Done in theory
}

// ----------------------------------------------------------------------------------
// Destructor
// Arguments, Returns:	(None)
// Description:			Cleans up and releases memory
// ----------------------------------------------------------------------------------
Source::~Source()
{
	std::vector<Genetics*>::iterator it;
	for( it = myGenomes.begin(); it != myGenomes.end(); it++ )
	{
		delete (*it);
	}

}

// ----------------------------------------------------------------------------------
// Method:		Prepare
// Arguments:	(None)
// Returns:		True on success
// Description:	Loads the genetics files into memory and performs the trimmings etc.
// ----------------------------------------------------------------------------------
bool Source::Prepare()
{
	//prepare all of the genetics files
	std::vector<Genetics*>::iterator it;
	for( it = myGenomes.begin(); it != myGenomes.end(); it++ )
	{
		(*it)->Prepare();
		std::vector<Gene*>::iterator itt;
		for( itt = mySearchMasks.begin(); itt != mySearchMasks.end(); itt++ )
		{
			(*it)->trimGenes(*itt,true);
			if (myCooerce)
			{
				(*it)->retype(myCooerceTo);
			}
		}
	}
	return true;
}

}

