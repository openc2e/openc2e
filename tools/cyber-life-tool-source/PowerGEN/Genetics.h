// --------------------------------------------------------------------------------------
// File:		Genetics.h
// Classes:		Gene, Genetics
// Purpose:		Storage and manipulation of the gene representations throughout NextGEN
// Description:	
//  The Gene structure stores the type information and ultimately the gene data itself.
//
//  The Genetics class manages these Gene structures, handling the merge etc.
//
// History:
//	16Jun99		DanS	Initial Version
//
// --------------------------------------------------------------------------------------

#ifndef GENETICS_H
#define GENETICS_H

#include "defs.h"

#include <list>
#include <vector>
#include <string>


#pragma once

namespace NextGEN
{

// Forward definition of GNOEntry and GNOFile for the structures below
struct GNOEntry;
class GNOFile;

// Gene structure.
struct Gene
{
	byte		variant;
	byte		group;
	byte		master;
	byte		gene;
	byte		subgroup;
	byte		subgene;
	std::vector<GNOEntry*> *gnoEntries;

	// This destructor is here purely to ensure that the GNO entries are deleted only
	// when explicitly done so by GNO managers.
	~Gene() { gnoEntries = NULL; };
};


class Genetics
{
private:
	// Attribute: myGenus - Stores the genus of the genetics file.
	Gene* myGenus;

	// Attribute: myVerbosity - Stores the verbosity level of the game,
	int myVerbosity;

public:
	// Attribute: myGeneticsFile - The name of then Gen file.
	std::string myGeneticsFile;

	// Attribute: myNormalisedName - The GEN name minus paths
	std::string myNormalisedName;

	// Attribute: myGenes - A list of Gene structures managed by the Genetics class.
	std::list<Gene*> myGenes;

	// Attribute: myGNO - A GNOFile relating directly to the genetics file.
	GNOFile *myGNO;

	// ----------------------------------------------------------------------------------
	// Constructor
	// Arguments:	const char* filename - The filename of the genetics to load
	//				int verbosity		 - The verbosity level to set.
	// Returns:		(None)
	// ----------------------------------------------------------------------------------
	Genetics(const char* filename, int verbosity);

	// ----------------------------------------------------------------------------------
	// Constructor
	// Arguments:	byte genusType		 - The creature type to create.
	//				char* filename		 - The name to assign initially
	//				int verbosity		 - The verbosity level to set initially
	// Returns:		(None)
	// ----------------------------------------------------------------------------------
	Genetics(byte genusType, char* filename, int verbosity);

	// ----------------------------------------------------------------------------------
	// Destructor
	// Arguments:	(None)
	// Returns:		(None)
	// ----------------------------------------------------------------------------------
	~Genetics();

	// ----------------------------------------------------------------------------------
	// Method:		trimGenes
	// Arguments:	Gene* mask			 - The gene mask to apply to the trimming process
	//				bool preserve		 - Whether we are trimming out, or to the mask.
	// Returns:		(None)
	// ----------------------------------------------------------------------------------
	void trimGenes(Gene* mask, bool preserve);

	// ----------------------------------------------------------------------------------
	// Method:		mergeIn
	// Arguments:	Genetics* from		 - The genetics file to merge genes from.
	//				Gene* mask			 - The merge mask used to perform the merge
	//				bool overwrite		 - Whether or not to overwrite existing genes.
	//										i.e. whether or not to pre-mask the genetics
	//				bool includename	 - Whether or not to append the name of the merge
	//										genetics to the name of our genetics.
	// Returns:		(None)
	// ----------------------------------------------------------------------------------
	void mergeIn(Genetics* from, Gene* mask,bool overwrite, bool includename);

	// ----------------------------------------------------------------------------------
	// Method:				Prepare
	// Arguments, Returns: (None)
	// Description:			Loads the genetics from the file.
	// ----------------------------------------------------------------------------------
	bool Prepare();

	void Genetics::RandomlyReorder();
	// ----------------------------------------------------------------------------------
	// Method:		blitOut
	// Arguments:	const char* foldername - The location to save the genetics to.
	// Returns:		(None)
	// Description: Saves the genetics data into the specified folder and matches it with
	//				the GNO data from the linked GNOFile class.
	// ----------------------------------------------------------------------------------
	void blitOut(const char* foldername);

	// ----------------------------------------------------------------------------------
	// Method:		retype
	// Arguments:	byte variant	- The variant to set all nonspecific genes to.
	// Returns:		(None)
	// Description: Resets all variant genes whose tag is zero (nonspecific) to the
	//				variant passed in the argument.
	// ----------------------------------------------------------------------------------
	void retype(byte variant);

};


//Nice definitions for Gene processing.
enum 
{
	BRAIN = 0,
	BIOCHEM = 1,
	CREATURE = 2,
	GAITPOSE = 3,
	BEHAVIOUR = 4
};

//BRAIN
enum 
{
	ORGANS = 0,
	LOBES = 1,
	TRACTS = 2,
};

//BIOCHEM
//const int ORGANS 0
//IN ORGANS
enum
{
	REACTS = 0,
	EMITS = 1,
	RECEPS = 2,
};

enum
{
	CONCS = 1,
	HALFS = 2,
};
//CREATURE
enum
{
	PARTS = 0,
	PIGS = 1,
	BLEEDS = 2,
	FACIALEXP = 3,
};

//GAITSNPOSES
enum
{
	GAITS = 0,
	POSES = 1,
};

//BEHAVIOUR
enum
{
	NEUROTRANS = 0,
	INSTS = 1,
	STIMS = 2,
};


}

#endif // GENETICS_H

