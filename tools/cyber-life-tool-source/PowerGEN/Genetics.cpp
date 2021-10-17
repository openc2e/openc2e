// --------------------------------------------------------------------------------------
// File:		Genetics.cpp
// Class:		Genetics
// Purpose:		Storage and manipulation of the gene representations throughout NextGEN
// Description:	
//  The Genetics class manages these Gene structures, handling the merge etc.
//
// History:
//	16Jun99		DanS	Initial Version
//
// --------------------------------------------------------------------------------------

#include "Genetics.h"
#include "GnoFile.h"
#include "../../common/FileScanner.h" //For the PATHSEP definitions
#include "ScriptExecutive.h"
#include <stdio.h>
#include <algorithm>

namespace NextGEN
{

std::string groups[] =
{
	"BRAIN",
	"BIOCHEM",
	"CREATURE",
	"GAITPOSE"
};

std::string masters[] =
{
	"ORGANS",
	"LOBES",
	"TRACTS",
	"INSTS",
	"STIMS",
	"NEUROTRANS",
	"PARTS",
	"PIGS",
	"BLEEDS",
	"GAITS",
	"POSES",
	"CONCS",
	"HALFS"
};

std::string subgroups[] =
{
	"REACTS",
	"EMITS",
	"RECEPS"
};


//The Genesize function returns the total storage requirements for the passed gene
//type/subtype pair.

int genesize(int type, int subtype)
{
	int ret = -1;
	if (type == 0 && subtype == 0) ret = 121;
	else if (type == 0 && subtype == 1) ret = 5;
	else if (type == 0 && subtype == 2) ret = 128;
	
	else if (type == 1 && subtype == 0) ret = 8;
	else if (type == 1 && subtype == 1) ret = 8;
	else if (type == 1 && subtype == 2) ret = 9;
	else if (type == 1 && subtype == 3) ret = 256;
	else if (type == 1 && subtype == 4) ret = 2;
	else if (type == 1 && subtype == 5) ret = 15;

	else if (type == 2 && subtype == 0) ret = 13;
	else if (type == 2 && subtype == 1) ret = 65;
	else if (type == 2 && subtype == 2) ret = 3;
	else if (type == 2 && subtype == 3) ret = 17;
	else if (type == 2 && subtype == 4) ret = 9;
	else if (type == 2 && subtype == 5) ret = 9;
	else if (type == 2 && subtype == 6) ret = 2;
	else if (type == 2 && subtype == 7) ret = 2;
	else if (type == 2 && subtype == 8) ret = 11;
	
	else if (type == 3 && subtype == 0) ret = 5;

	//Any other genes should __not__ exist, so throw a strop.
	else
		throw NULL;

	return ret+12+sizeof(Gene);
}

// ----------------------------------------------------------------------------------
// Constructor
// Arguments:	const char* filename - The filename of the genetics to load
//				int verbosity		 - The verbosity level to set.
// Returns:		(None)
// ----------------------------------------------------------------------------------
Genetics::Genetics(const char *filename, int verbosity)
{
	if (verbosity > 4)
		printf("Preparing genetics file %s\n",filename);
	//Failure to load the file here indicates serious problems...
	// as such, we care not for the troubles of exceptions etc.
	// let them die - that's what I say

	char buffer[27];

	FILE *fd;
	fd = fopen(filename,"rb");
	if (fd == NULL)
	{
		throw NULL;
	}

	fread(&buffer[0],27,1,fd);
	fclose(fd);
	//check for validity...
	if (
		buffer[0] != 'd' ||
		buffer[1] != 'n' ||
		buffer[2] != 'a' ||
		buffer[3] != '3' ||
		buffer[4] != 'g' ||
		buffer[5] != 'e' ||
		buffer[6] != 'n' ||
		buffer[7] != 'e' ||
		buffer[8] != 2 ||
		buffer[9] != 1 ||
		buffer[10] != 1
		)
	{
		printf("Hmm, %s is not a valid C3/C2e genetics file!\n",filename);
		throw NULL;
	}
	//Okay, we will be able to pulse this into memory...
	myGeneticsFile.assign(filename);
	myVerbosity = verbosity;
	//hmm...
	int lastsep = myGeneticsFile.find_last_of(PATHSEPS);
	myNormalisedName.assign(myGeneticsFile.substr(lastsep+1,myGeneticsFile.length()-lastsep));
	char copyofname[1024];
	strcpy(&copyofname[0],filename);
	myGNO = new GNOFile(copyofname);
}

// ----------------------------------------------------------------------------------
// Constructor
// Arguments:	byte genusType		 - The creature type to create.
//				char* filename		 - The name to assign initially
//				int verbosity		 - The verbosity level to set initially
// Returns:		(None)
// ----------------------------------------------------------------------------------
Genetics::Genetics(byte genusType, char *filename, int verbosity)
{
	myGeneticsFile.assign(filename);
	myVerbosity = verbosity;
	
	//hmm...
	int lastsep = myGeneticsFile.find_last_of(PATHSEPS);
	myNormalisedName.assign(myGeneticsFile.substr(lastsep+1,myGeneticsFile.length()-lastsep));
	
	myGenus = (NextGEN::Gene*)malloc(genesize(2,1));
	myGenus->gnoEntries = new std::vector<GNOEntry*>;
	char* gene = (char*)myGenus+sizeof(NextGEN::Gene);
	memcpy(gene,"gene\x02\x01\x01\x00\x00\x00\x80\x00\x00Next GENNext GENNext GENNext GENNext GENNext GENNext GENNext GEN",genesize(2,1)-sizeof(Gene));
	gene[12] = genusType;
	myGNO = NULL;
}


// ----------------------------------------------------------------------------------
// Destructor
// Arguments:	(None)
// Returns:		(None)
// ----------------------------------------------------------------------------------
Genetics::~Genetics()
{
	free(myGenus);
	std::list<Gene*>::iterator it;
	for(it = myGenes.begin(); it != myGenes.end(); it++ )
		free(*it);
}


// ----------------------------------------------------------------------------------
// Method:		trimGenes
// Arguments:	Gene* mask			 - The gene mask to apply to the trimming process
//				bool preserve		 - Whether we are trimming out, or to the mask.
// Returns:		(None)
// ----------------------------------------------------------------------------------
void Genetics::trimGenes(Gene *mask, bool preserve)
{
	//preserve is the "remove" / "leave" flag.
	//if (preserve) remove(!match) else remove(match);
	std::list<Gene*>::iterator it;
	for(it = myGenes.begin(); it != myGenes.end(); it++)
	{
		Gene* aGene = *it;
		bool found =  (
			(mask->variant == 255 || mask->variant == aGene->variant) &&
			(mask->group == 255 || mask->group == aGene->group) &&
			(mask->master == 255 || mask->master == aGene->master) &&
			(mask->gene == 255 || mask->gene == aGene->gene) &&
			(mask->subgroup == 255 || mask->subgroup == aGene->subgroup) &&
			(mask->subgene == 255 || mask->subgene == aGene->subgene)
			);
		//Right, we know if we have a match.
		//We delete the gene under the following circumstances...
		// (preserve && !found) || (!preserve && found)
		if ((preserve && !found) || (!preserve && found))
		{
			if (myVerbosity > 6)
				printf("Deleting a gene (%d %d %d %d %d %d : %d)\n",aGene->variant,aGene->group,aGene->master,aGene->gene,aGene->subgroup,aGene->subgene,aGene->gnoEntries->size());
			it++;
			myGenes.remove(aGene);
			delete aGene;
			it--;
		}
	}
}


// sizeofa Returns the size of the gene type passed to it.
int sizeofa(Gene* b)
{
	byte* c = (byte*)b;
	c += sizeof(Gene);
	return genesize(c[4],c[5]);
}

//SetGene sets the current gene to the specified settings iff it matches the type/subtype
// Also, if the gene turns out to be an ORGAN then curorgan is set.
inline void SetGene(Gene* aGene, char head[], byte *curorgan, byte a, byte b, byte c, byte d) 
{
	if (head[4] == a && head[5] == b) 
	{
		aGene->group = c;
		aGene->master = d;
		aGene->gene = head[6];
		if (d == ORGANS && c == BIOCHEM) *curorgan = head[6];
	}
}

//SetBioGene does the same job as SetGene but only for biochemical genes
// I.E. only those for whom the organ matters.
inline void SetBioGene(Gene *aGene, char head[], byte curorgan, byte a, byte b, byte c, byte d) 
{
	if (head[4] == a && head[5] == b) 
	{
		aGene->group = c;
		aGene->master = ORGANS;
		aGene->gene = curorgan;
		aGene->subgroup = d;
		aGene->subgene = head[6];
	}
}



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
void Genetics::mergeIn(Genetics *from, Gene *mask, bool overwrite, bool includename)
{
	
 	if (overwrite)
		trimGenes(mask,false);
	
	//Append ".<from->normalisedname>" to my normalisedname if we need to :)
	if (includename) { myNormalisedName.append("."); myNormalisedName.append(from->myNormalisedName); }
	if (myVerbosity > 6)
		printf("Name now: %s\n",myNormalisedName.c_str());
	//next, for each gene in from, overwrite it if it exists, else insert it.
	std::list<Gene*>::iterator mergegenes;
	if (myVerbosity>4)
		printf("There are %d genes to add into my %d strong pool\n",from->myGenes.size(),myGenes.size());
	for(mergegenes = from->myGenes.begin(); mergegenes != from->myGenes.end(); mergegenes++)
	{
		Gene* aGene;
		aGene = NULL;
		std::list<Gene*>::iterator oGene;
		oGene = NULL;
		std::list<Gene*>::iterator it;
		// Now try to find the Gene in my lists. At the same time, record the Organ to put it in.
		for (it = myGenes.begin(); it != myGenes.end(); it++ )
		{
			if (
				((*mergegenes)->variant == (*it)->variant) &&
				((*mergegenes)->group == (*it)->group) &&
				((*mergegenes)->master == (*it)->master) &&
				((*mergegenes)->gene == (*it)->gene) &&
				((*mergegenes)->subgroup == (*it)->subgroup) &&
				((*mergegenes)->subgene == (*it)->subgene)
				)
				aGene = *it;
			if (((*mergegenes)->group == (*it)->group) &&
				((*mergegenes)->master == (*it)->master) &&
				((*mergegenes)->gene == (*it)->gene) &&
				((*it)->subgroup == 255) &&
				((*it)->subgene == 255) &&
				((*mergegenes)->subgroup != 255) &&
				((*mergegenes)->subgene != 255)
				)
				oGene = ((++it)--); // Save the organ for later if we need it :)
		}
		if (aGene != NULL)
		{
			//Here we simply replace the one gene with the other...
			memcpy(aGene,*mergegenes,sizeofa(aGene));
			//CunningBosh(*mergegenes,aGene);
			if (myVerbosity > 6)
				printf("Replaced a gene (%d %d %d %d %d %d : %d)\n",aGene->variant,aGene->group,aGene->master,aGene->gene,aGene->subgroup,aGene->subgene,aGene->gnoEntries->size());
		}
		else
		{
			//Here we have the hard work of inserting the gene into the correct place
			//within the gene set...
			//This is actually remarkably simple unless we are dealing with a biochem
			//gene in which case we have to insert it directly after the organ in question
			//to ensure we preserve the ordering within the genome.
			//In an attempt to circumvent problems, let's actually copy the gene...

			aGene = *mergegenes;
			Gene *cGene;
			cGene = (Gene*)malloc(sizeofa(aGene));
			memcpy(cGene,aGene,sizeofa(aGene));
			//CunningBosh(aGene,cGene);
			aGene=cGene;
			if (aGene->subgroup != 255)
			{
				//We have an organ situation on our hands
				//So insert it directly after "oGene" // Oh fner! We have a problem houston!?!??!
				if (oGene == NULL)
				{
					printf("Erkle, organ failure!\n");
					printf("%d: %d %d %d %d %d %d (%d)\n",aGene,aGene->variant,aGene->group,aGene->master,aGene->gene,aGene->subgroup,aGene->subgene,aGene->gnoEntries->size());
					exit(1);
				}
				else
				{
					std::list<Gene*>::iterator rGene = oGene;
					--oGene; // oGene is now the Organ Gene
					// ((rGene != myGenes.end()) && ((*rGene)->subgroup == REACTS) && (aGene->subgroup != REACTS))
					// aGene is the source gene.
					// oGene is the Organ it belongs to
					// rGene is the gene directly after the organ gene.
					bool atEnd = (rGene == myGenes.end());
					bool rGeneIsAReact = atEnd?false:(*rGene)->subgroup == REACTS;
					bool aGeneIsAReact = atEnd?false:(aGene->subgroup == REACTS);
			
					if (rGeneIsAReact) // These don't ever fire if at end of genome.
						int c=0;
					if (aGeneIsAReact) // These don't ever fire if at end of genome.
						int d=0;
					if (atEnd)
						int e=0;
					if (!atEnd && rGeneIsAReact && !aGeneIsAReact)
					{
						myGenes.insert(++rGene,aGene);
//						printf("R: %d %d %d %d %d %d (%d)\n",aGene->variant,aGene->group,aGene->master,aGene->gene,aGene->subgroup,aGene->subgene,aGene->gnoEntries->size());
					}
					else
					{
						myGenes.insert(++oGene,aGene);
//						printf("O: %d %d %d %d %d %d (%d)\n",aGene->variant,aGene->group,aGene->master,aGene->gene,aGene->subgroup,aGene->subgene,aGene->gnoEntries->size());
					}
				}
			}
			else
				myGenes.push_back(aGene);

			if (myVerbosity > 6)
				printf("Inserted a gene (%d %d %d %d %d %d : %d)\n",aGene->variant,aGene->group,aGene->master,aGene->gene,aGene->subgroup,aGene->subgene,aGene->gnoEntries->size());
		}
	}
	if (myVerbosity>4)
	printf("Done Merging\n");

}

// ----------------------------------------------------------------------------------
// Method:				Prepare
// Arguments, Returns:	(None)
// Description:			Loads the genetics from the file.
// ----------------------------------------------------------------------------------
bool Genetics::Prepare()
{
	//Simply load the genetics in here... Failure will __NOT__ be tolerated again, throwing NULL
	bool eof = false;
	FILE *fd;
	fd = fopen(myGeneticsFile.c_str(),"rb");
	char head[12];
	fread(&head[0],4,1,fd);
	Gene* aGene;
	aGene = (Gene*)malloc(genesize(2,1));
	aGene->gnoEntries = new std::vector<GNOEntry*>;
	int amt = genesize(2,1);
	amt -= sizeof(Gene);
	fread((++aGene)--,amt,1,fd);
	//aGene--;
	myGenus = aGene;
	byte curorgan;
	while (!eof)
	{
		//Load one gene in...
		fread(&head[0],4,1,fd);
		if (head[0] == 'g' &&
			head[1] == 'e' &&
			head[2] == 'n' &&
			head[3] == 'e')
		{
			//we have a gene...
			if (!fread(&head[4],8,1,fd))
			{
				printf("Why not reading the header??!? feof()=%d ferror()=%d",feof(fd),ferror(fd));
				throw NULL;
			}
			aGene = (Gene*)malloc(genesize(head[4],head[5]));
			aGene->gnoEntries = new std::vector<GNOEntry*>;
			aGene->variant = head[11];
			aGene->subgene = 255;
			aGene->subgroup = 255;

			SetGene(aGene,head,&curorgan,0,0,BRAIN,LOBES);
			SetGene(aGene,head,&curorgan,0,1,BRAIN,ORGANS);
			SetGene(aGene,head,&curorgan,0,2,BRAIN,TRACTS);
			
			SetGene(aGene,head,&curorgan,3,0,BIOCHEM,ORGANS);
			SetBioGene(aGene,head,curorgan,1,2,BIOCHEM,REACTS);
			SetBioGene(aGene,head,curorgan,1,1,BIOCHEM,EMITS);
			SetBioGene(aGene,head,curorgan,1,0,BIOCHEM,RECEPS);
			SetGene(aGene,head,&curorgan,1,4,BIOCHEM,CONCS);
			SetGene(aGene,head,&curorgan,1,3,BIOCHEM,HALFS);
			
			SetGene(aGene,head,&curorgan,2,2,CREATURE,PARTS);
			SetGene(aGene,head,&curorgan,2,6,CREATURE,PIGS);
			SetGene(aGene,head,&curorgan,2,7,CREATURE,BLEEDS);
			SetGene(aGene,head,&curorgan,2,8,CREATURE,FACIALEXP);
			
			SetGene(aGene,head,&curorgan,2,4,GAITPOSE,GAITS);
			SetGene(aGene,head,&curorgan,2,3,GAITPOSE,POSES);

			SetGene(aGene,head,&curorgan,1,5,BEHAVIOUR,NEUROTRANS);
			SetGene(aGene,head,&curorgan,2,5,BEHAVIOUR,INSTS);
			SetGene(aGene,head,&curorgan,2,0,BEHAVIOUR,STIMS);
			//All configured now...
			memcpy((++aGene)--,&head[0],12);
			//read it in
			char* loc = (char*)((++aGene)--);
			for (int newloop=0;newloop<12;newloop++)
				loc++;
			int amountToReadIn = genesize(head[4],head[5])-sizeof(Gene)-12;
			if (!fread(loc, amountToReadIn, 1, fd))
			{
				printf("Why not reading the data????");
				throw NULL;
			}

			//gtb! new flag for producing genomes with random data:
			if (theReplaceByRandomDataFlag) {
				for (int i=0; i<amountToReadIn; i++) {
					loc[i] = (char)(rand());
				}
			}


			//Dump aGene into the genes list
			if (myVerbosity > 7)
				printf("Dealt with gene %d %d %d\n",head[4],head[5],head[6]);
			myGNO->LinkGene(aGene);
			myGenes.push_back(aGene);
		}
		else if (head[0] == 'g' &&
			head[1] == 'e' &&
			head[2] == 'n' &&
			head[3] == 'd')
			eof = true;
		else
		{
			printf("loaded %d genes, now fecked! head is %s\n",myGenes.size(),head);
			throw NULL;
		}
	}
	fclose(fd);
	if (myVerbosity > 4)
		printf("loaded %d genes from %s\n",myGenes.size(),myGeneticsFile.c_str());
	return true;
}


//WriteStr writes a string out to the fd (File descriptor)
inline void WriteStr(FILE *fd,std::string a)
{
	int j;
	j=a.length();
	fwrite(&j,2,1,fd);
	if (j!=0) 
		fwrite(a.c_str(),j,1,fd);
}

void Genetics::RandomlyReorder()
{
	// random_shuffle doesn't compile when applied to a list directly:
	// std::random_shuffle(myGenes.begin(), myGenes.end());
	// so we transfer to a vector temporarily
	printf("Randomly shuffling...");
	std::vector<Gene*> tempGenes;
	tempGenes.resize(myGenes.size());
	std::copy(myGenes.begin(), myGenes.end(), tempGenes.begin());
	std::random_shuffle(tempGenes.begin(), tempGenes.end());
	std::copy(tempGenes.begin(), tempGenes.end(), myGenes.begin());

}

// ----------------------------------------------------------------------------------
// Method:		blitOut
// Arguments:	const char* foldername - The location to save the genetics to.
// Returns:		(None)
// Description: Saves the genetics data into the specified folder and matches it with
//				the GNO data from the linked GNOFile class.
// ----------------------------------------------------------------------------------
void Genetics::blitOut(const char *foldername)
{
	//First task, scan through the myGenes from start to end, performing an iterative
	//removal/resequencing task.
	//if a gene is duplicated in type/subtype/id then all duplicates must
	//be removed, and the first one set to variant zero (common)
	std::list<Gene*>::iterator it;
	std::list<Gene*>::iterator anotherit;
	bool donesome;

	if (myVerbosity > 2)
		printf("Performing coalescence on %s...\n",myNormalisedName.c_str());
	for(it = myGenes.begin(); it != myGenes.end(); it++)
	{
		donesome = false;
		for(anotherit = (++it)--; anotherit != myGenes.end(); anotherit++)
		{
			if ((*anotherit)->group == (*it)->group &&
				(*anotherit)->master == (*it)->master &&
				(*anotherit)->gene == (*it)->gene &&
				(*anotherit)->subgene == (*it)->subgene &&
				(*anotherit)->subgroup == (*it)->subgroup)
			{
				if (myVerbosity > 6)
				{
					printf("Found a gene to remove... %d %d %d %d %d %d : %d\n",(*anotherit)->variant,(*anotherit)->group,(*anotherit)->master,(*anotherit)->gene,(*anotherit)->subgroup,(*anotherit)->subgene,(*anotherit)->gnoEntries->size());
				}
				donesome = true;
				Gene* meh = *anotherit;
				anotherit++;
				myGenes.remove(meh);
				delete meh;
				anotherit--;
			}
		}
		if (donesome)
		{
			(*it)->variant = 0;
			if (myVerbosity > 4)
				printf("Coalescence for %d %d %d %d %d %d : %d completed.\n",(*it)->variant,(*it)->group,(*it)->master,(*it)->gene,(*it)->subgroup,(*it)->subgene,(*it)->gnoEntries->size());
		}
	}

	if (theRandomlyReorderFlag)
		RandomlyReorder();
	
	std::string filename;
	filename = foldername;

	filename.append(myNormalisedName);

	if (myVerbosity > 1)
		printf("Writing to %s\n",filename.c_str());
	FILE* fd = NULL;
	if ((fd = fopen(filename.c_str(),"wb"))==NULL)
	{
		printf("Open failure on %s\n",filename.c_str());
		exit(2);
	}
	fwrite("dna3",4,1,fd);
	fwrite(++myGenus,genesize(2,1)-sizeof(Gene),1,fd);
	for( it = myGenes.begin(); it != myGenes.end(); it++)
	{
		byte *ptr;
		ptr = (byte*)(*it);
		ptr += sizeof(Gene);
		fwrite(ptr,genesize(ptr[4],ptr[5])-sizeof(Gene),1,fd);
	}
	fwrite("gend",4,1,fd);
	fclose(fd);

	//Now we prepare and write out the GNO entries associated with the genetics.
	
	filename.at(filename.length()-2) = 'n';
	filename.at(filename.length()-1) = 'o';
	if ((fd = fopen(filename.c_str(),"wb")) == NULL)
	{
		printf("Open failure on %s\n",filename.c_str());
		exit(3);
	}
	int i,j,n;
	n = 2;
	fwrite(&n,2,1,fd);
	std::vector<GNOEntry*> vc;
	std::vector<GNOEntry*>::iterator pit;
	for(anotherit = myGenes.begin(); anotherit != myGenes.end(); anotherit++)
	{
		for(pit = (*anotherit)->gnoEntries->begin(); pit != (*anotherit)->gnoEntries->end(); pit++)
		{
			if ((*pit)->noteType == 0)
				vc.push_back(*pit);
		}
	}
	n = vc.size();
	fwrite(&n,2,1,fd);
	for(i=0;i<n;i++)
	{
		//Write an SVNoteStruct
		j = vc.at(i)->type;
		fwrite(&j,2,1,fd);
		j = vc.at(i)->subtype;
		fwrite(&j,2,1,fd);
		j = vc.at(i)->id;
		fwrite(&j,2,1,fd);
		j = vc.at(i)->rule;
		fwrite(&j,2,1,fd);
		j = 0;
		fwrite(&j,2,1,fd);
		WriteStr(fd,vc.at(i)->entry0);
		WriteStr(fd,vc.at(i)->entry1);
		WriteStr(fd,vc.at(i)->entry2);
		WriteStr(fd,vc.at(i)->entry3);
		WriteStr(fd,vc.at(i)->entry4);
		WriteStr(fd,vc.at(i)->entry5);
		WriteStr(fd,vc.at(i)->entry6);
		WriteStr(fd,vc.at(i)->entry7);
		WriteStr(fd,vc.at(i)->entry8);
		WriteStr(fd,vc.at(i)->entry9);
		WriteStr(fd,vc.at(i)->entry10);
		WriteStr(fd,vc.at(i)->entry11);
		WriteStr(fd,vc.at(i)->entry12);
		WriteStr(fd,vc.at(i)->entry13);
		WriteStr(fd,vc.at(i)->entry14);
		WriteStr(fd,vc.at(i)->entry15);
		j = 0;
		fwrite(&j,2,1,fd);
		WriteStr(fd,vc.at(i)->entry16);
	}
	//Write out the topup count...
	j=0;
	for (i=0;i<(((1801)-n)*23);i++)
		fwrite(&j,2,1,fd);
	//clear and cleanup
	vc.clear();
	n = 2;
	fwrite(&n,2,1,fd);

	GNOEntry *myGNOEntry = new GNOEntry();
	myGNOEntry->noteType = 0;
	myGNOEntry->type = 2;
	myGNOEntry->subtype = 1;
	myGNOEntry->id = 1;
	myGNOEntry->entry0 = "001 - THE HEADER - COMPULSORY";
	myGNOEntry->entry1 = "";
	vc.push_back(myGNOEntry);

	//Now do other notes.
	for(anotherit = myGenes.begin(); anotherit != myGenes.end(); anotherit++)
	{
		for(pit = (*anotherit)->gnoEntries->begin(); pit != (*anotherit)->gnoEntries->end(); pit++)
		{
			if ((*pit)->noteType == 1)
				vc.push_back(*pit);
		}
	}
	n = vc.size();
	fwrite(&n,2,1,fd);
	for(i=0;i<n;i++)
	{
		//Write an SVNoteStruct
		j = vc.at(i)->type;
		fwrite(&j,2,1,fd);
		j = vc.at(i)->subtype;
		fwrite(&j,2,1,fd);
		j = vc.at(i)->id;
		fwrite(&j,2,1,fd);
		j = 0;
		fwrite(&j,2,1,fd);
		WriteStr(fd,vc.at(i)->entry0);
		WriteStr(fd,vc.at(i)->entry1);
	}
	j=0;
	for (i=0;i<(((1801)-n)*6);i++)
		fwrite(&j,2,1,fd);
	//clear and cleanup

	vc.clear();
	n = 0;
	fwrite(&n,2,1,fd);
	fclose(fd);
}


// ----------------------------------------------------------------------------------
// Method:		retype
// Arguments:	byte variant	- The variant to set all nonspecific genes to.
// Returns:		(None)
// Description: Resets all variant genes whose tag is zero (nonspecific) to the
//				variant passed in the argument.
// ----------------------------------------------------------------------------------
void Genetics::retype(byte variant)
{
	std::list<Gene*>::iterator it;
	if (myVerbosity > 3)
		printf("Re-typing genes to variant %d.\n",variant);
	for(it = myGenes.begin(); it != myGenes.end(); it++ )
	{
		if ((*it)->variant == 0)
			(*it)->variant = variant;
	}
}


}

