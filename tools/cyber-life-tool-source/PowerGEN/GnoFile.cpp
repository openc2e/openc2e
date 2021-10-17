// --------------------------------------------------------------------------------------
// File:		GnoFile.cpp
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

#include "GnoFile.h"
#include "defs.h"
#include "Genetics.h"

#include <vector>
#include <string>
#include <stdio.h>

namespace NextGEN
{



//ReadString reads a string, managing the VB string formats etc.
inline void ReadString(char *mybuffer, std::string *a, FILE *fd)
{
	int ln;
	a->assign(""); ln = 0;
	fread(&ln,2,1,fd);

	if (ln > 4050)
	{
		printf("Erkle ran out of GNO description space - longer than 4050 bytes - %d\n",ln);
		exit(4);
	}
	if (ln != 0)
	{
		fread(mybuffer,ln,1,fd);
		mybuffer[ln] = 0;
		a->assign(mybuffer,ln);
	}
}


// ----------------------------------------------------------------------------------
// Constructor
// Arguments:	char* filename	- The filename of the GNO file to load.
// Returns:		(None)
// Description:	Loads the GNO file into memory and generates the GNOEntry structures
// ----------------------------------------------------------------------------------
GNOFile::GNOFile(char *filename)
{
	//Load the gno file in here...
	std::string str;
	str = filename;
	str.at(str.length()-2) = 'n';  //patch .gen
	str.at(str.length()-1) = 'o';  //   to .gno

	char mybuffer[4096];

	FILE *fd;
	if ((fd = fopen(str.c_str(),"rb")) == NULL)
	{
		//Unable to open GNO file, so no gene notes here thankee
		return;
	}
	//GNO file opened, let's play mommy and load them all in
	//SVNotes and Notes are loaded into the same structure.
	//SVNotes are set with notetype 0 and notes with notetype 1
	//This is so that during the save process, the notetype can be used
	//to stream the correct ones out.
	//The GNO file is actually saved by the Genetics file class.

	int p;
	int loop,loopmax;
	loopmax = 0; loop = 0; p = 0;
	GNOEntry *gnoe;
	fread(&p,2,1,fd);

	fread(&loopmax,2,1,fd);

	for(loop=0;loop<1801;loop++)	//To 1801 to handle VB structures
	{
		//Read an SVStruct into a CGNOEntry
		gnoe = new GNOEntry();
		gnoe->noteType = 0;
		gnoe->id = gnoe->type = gnoe->subtype = gnoe->rule = 0;
		fread(&gnoe->type,2,1,fd);
		fread(&gnoe->subtype,2,1,fd);
		fread(&gnoe->id,2,1,fd);
		fread(&gnoe->rule,2,1,fd);
		fread(&p,2,1,fd);
		//16 entries...
		ReadString(&mybuffer[0],&(gnoe->entry0),fd);
		ReadString(&mybuffer[0],&(gnoe->entry1),fd);
		ReadString(&mybuffer[0],&(gnoe->entry2),fd);
		ReadString(&mybuffer[0],&(gnoe->entry3),fd);
		ReadString(&mybuffer[0],&(gnoe->entry4),fd);
		ReadString(&mybuffer[0],&(gnoe->entry5),fd);
		ReadString(&mybuffer[0],&(gnoe->entry6),fd);
		ReadString(&mybuffer[0],&(gnoe->entry7),fd);
		ReadString(&mybuffer[0],&(gnoe->entry8),fd);
		ReadString(&mybuffer[0],&(gnoe->entry9),fd);
		ReadString(&mybuffer[0],&(gnoe->entry10),fd);
		ReadString(&mybuffer[0],&(gnoe->entry11),fd);
		ReadString(&mybuffer[0],&(gnoe->entry12),fd);
		ReadString(&mybuffer[0],&(gnoe->entry13),fd);
		ReadString(&mybuffer[0],&(gnoe->entry14),fd);
		ReadString(&mybuffer[0],&(gnoe->entry15),fd);
		fread(&p,2,1,fd);
		ReadString(&mybuffer[0],&(gnoe->entry16),fd);

		if (loop<loopmax)
			myGnoEntries.push_back(gnoe);
		else
			delete gnoe;				//Ignore the loaded note if it is not allocated.
	}
	fread(&p,2,1,fd);

	fread(&loopmax,2,1,fd);


	for(loop=0;loop<1801;loop++)
	{
		//read a notestruct into a CGNOEntry
		gnoe = new GNOEntry();
		gnoe->noteType = 1;
		fread(&gnoe->type,2,1,fd);
		fread(&gnoe->subtype,2,1,fd);
		fread(&gnoe->id,2,1,fd);
		fread(&p,2,1,fd);

		//Only 2 entries
		ReadString(&mybuffer[0],&(gnoe->entry0),fd);
		ReadString(&mybuffer[0],&(gnoe->entry1),fd);

		if (loop<loopmax)
			myGnoEntries.push_back(gnoe);
		else
			delete gnoe;
	}
	fread(&p,2,1,fd);

	fclose(fd);
}

// ----------------------------------------------------------------------------------
// Destructor
// Arguments:	(None)
// Returns:		(None)
// Description:	Cleans up the GNO entries associated with this GNOFile
// ----------------------------------------------------------------------------------
GNOFile::~GNOFile()
{
	//In theory nothing - unsure otherwise
	//This is because the vector throws the data away for us.
}


// ----------------------------------------------------------------------------------
// Method:		LinkGene
// Arguments:	Gene* thisgene	- The gene to link all GNO entries who are associated
// Returns:		(None)
// Description:	Links all relevant GNO entries from this file to the gene passed.
// ----------------------------------------------------------------------------------
void GNOFile::LinkGene(Gene *thisgene)
{
	//This is where we find the GNO entries and add them to the CGene stuffs.
	//What we do is to find each GNO entry which matches, and then we duplicate them
	//dumping them into the CGene gnoEntries structure.
	
	GNOEntry *gnoe;

	if (thisgene->gnoEntries == NULL)
		thisgene->gnoEntries = new std::vector<GNOEntry*>;
	else
		thisgene->gnoEntries->clear();

	//First off, dereference the CGene structure and pull out the genetics information.

	byte *bp;
	bp = (byte*)(thisgene);
	bp = bp + sizeof(Gene);
	
	std::vector<GNOEntry*>::iterator it;
	for(it = myGnoEntries.begin(); it != myGnoEntries.end(); it++)
	{
		if ((*it)->type == bp[4] &&
			(*it)->subtype == bp[5] &&
			(*it)->id == bp[6])
		{
			gnoe = new GNOEntry();
			
			gnoe->entry0 = (*it)->entry0;
			gnoe->entry1 = (*it)->entry1;			
			gnoe->entry2 = (*it)->entry2;			
			gnoe->entry3 = (*it)->entry3;			
			gnoe->entry4 = (*it)->entry4;			
			gnoe->entry5 = (*it)->entry5;			
			gnoe->entry6 = (*it)->entry6;			
			gnoe->entry7 = (*it)->entry7;			
			gnoe->entry8 = (*it)->entry8;			
			gnoe->entry9 = (*it)->entry9;			
			gnoe->entry10 = (*it)->entry10;			
			gnoe->entry11 = (*it)->entry1;			
			gnoe->entry12 = (*it)->entry12;			
			gnoe->entry13 = (*it)->entry13;			
			gnoe->entry14 = (*it)->entry14;			
			gnoe->entry15 = (*it)->entry15;			
			gnoe->entry16 = (*it)->entry16;

			gnoe->noteType = (*it)->noteType;
			gnoe->type = (*it)->type;
			gnoe->subtype = (*it)->subtype;
			gnoe->id = (*it)->id;
			gnoe->rule = (*it)->rule;
			thisgene->gnoEntries->push_back(gnoe);
		}
	}
	
}


}

