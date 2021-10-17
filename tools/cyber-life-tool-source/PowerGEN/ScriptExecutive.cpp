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

#include "ScriptExecutive.h"
#include "../../common/FileScanner.h"
#include "Source.h"
#include "Genetics.h"
#include "GnoFile.h"
#include "PowerGen.h"

#include "defs.h"

#include <time.h>
#include <stdio.h>



bool theReplaceByRandomDataFlag = false;
bool theRandomlyReorderFlag = false;



namespace NextGEN
{

char* genusname[] =
{
	"norn",
    "gren",
	"ettn"
};

char* genusletters[] = 
{
	"n",
	"g",
	"e"
};

int geneGeneration = 0;
const std::string numberingDictionary = "abcdefghijklmnopqrstuvwxyz0123456789+-.`!\"£$%^&()[]{},;#~";


//The generation gap is for managing splices between 90 & 00 where the next number _ought_ to be 01 not 91
const int generationGap = 20;


// The aliases used by the system,
std::string aliases[] = 
{
	".\\",		//0
	"",			//1
	"",			//2
	"",			//3
	"",			//4
	"",			//5
	"",			//6
	"",			//7
	"",			//8
	""			//9
};

//Helper Functions

// --------------------------------------------------------------------------------------
// Method:		expandFileRef
// Arguments:	char* filname - The filename complete with aliases to expand.
// Returns:		std::string containing expanded reference
// Description:	Uses the alias expansion method to expand $n aliases in filename
// --------------------------------------------------------------------------------------
std::string expandFileRef(char* filename)
{
	std::string retval;
	retval.assign(filename);
	bool used[10];
	int loop;
	for (loop=0;loop<10;loop++)
		used[loop] = false;
	while (retval.find_first_of("$") != -1)
	{
		int posof = retval.find_first_of("$");
		int alias = (retval.at(posof+1)) - '0';
		if (alias<0 || alias>9)
		{
			printf("Oik! Unknown alias in %s\n",filename);
			exit(1);		//Unceremoniousley dump for now, may rewrite later
		}
		if (used[alias])
		{
			printf("Hey! I hate multiple references!!!\nPotentially circular reference detected in %s\n",filename);
			exit(2);		//Unceremoniousley dump for now, may rewrite later
		}
		used[alias] = true;
		retval.replace(posof,2,aliases[alias]);
	}
	char* text = PATHSEPSD;
	while (retval.find(text) != -1)
	{
		int pos = retval.find(text);
		retval.replace(pos,1,"");
	}
	return retval;
}

void buildAlias(int alias, char *pathfrom,bool isfile, int verb)
{
	//There are 2 stages to the alias building functions.
	//1. copy alias from pathfrom into aliases
	//2. ensure alias ends in PATHSEP
	aliases[alias].assign(pathfrom);
	if (isfile)
	{
		//Here we cheat slightly as we know that the alias is a file,
		//so we merely trim everything from the last PATHSEP off the string...
		try
		{
			int last = aliases[alias].find_last_of(PATHSEPS);
			int leng = aliases[alias].length();
			std::string subs = aliases[alias].substr(0,last+1);
			if (last == -1)
			{
				subs = ".";
				subs.append(PATHSEPS);
			}
			aliases[alias].assign(subs);
		}
		catch ( ... )
		{
			printf("Strange, an exception parsing the alias...");
		}
	}
	else
		if (aliases[alias].at((aliases[alias].length())-1) != PATHSEP)
		{
			aliases[alias].append(PATHSEPS);
		}
	if (verb > 3)
		printf("Alias set for %d to %s\n",alias,aliases[alias].c_str());
}


// ----------------------------------------------------------------------------------
// Constructor
// Arguments:	char* filename - The filename of the script file.
//				int verbosity  - The verbosity of the environment (requested by user)
// Returns:		(None)
// Description:	Parses the script, and readies it for execution.
// ----------------------------------------------------------------------------------
ScriptExecutive::ScriptExecutive(char *filename, int verbosity)
{
	myVerbosityLevel = verbosity;
	myOutputMode = 1;
	if (verbosity > 0)
		printf("Processing script file %s at verbosity level %d\n",filename,verbosity);

	//This function has to manage the files...
	FILE *scriptFile;

	//Initialises aliases 0 to CWD (automatically done as ".")
	// and 1 to SWD (Hmm...)

	if (verbosity > 2)
		printf("Building SWD alias\n");

	buildAlias(1,filename,true,verbosity);

	if (verbosity > 2)
		printf("Opening file...\n");
	if ((scriptFile = fopen(filename,"r")) == NULL)
	{
		//An error occurred whilst opening the script file for input.
		//Failure mode active...
		//To describe failure mode we simply state that the filename
		//copied is "***"
		printf("Unable to open script File %s\n",filename);
		mySourceName = "***";
		return;
	}

	//We opened the sourcefile, so process it...
	if (verbosity > 2)
	{
		printf("File opened, working...\n");
	}
	char scriptLine[100];

	while (!feof(scriptFile))
	{
		if (verbosity > 8)
			printf("--");
		if (NULL == fgets(&scriptLine[0],100,scriptFile) && !feof(scriptFile))
		{
			//An error occurred yet again, so fail once more....
			printf("Error occurred reading text from %s\n",filename);
			mySourceName = "***";
			fclose(scriptFile);
			return;
		}
		//Line loaded... Search for "\n" and replace it
		if (verbosity > 7)
			printf("::%s",scriptLine);
		bool found = false;
		for(int i=0;i<100;i++)
		{
			if (scriptLine[i] == '\n')
			{
				scriptLine[i] = 0;
				found = true;
			}
			if (scriptLine[i] == 0)
				i = 100;
		}
		if (!found && !feof(scriptFile))
		{
			printf("Line too long in %s\n",filename);
			printf("Start of line is: %s\n",scriptLine);
			
			mySourceName = "***";
			fclose(scriptFile);
			return;
		}
		if (!found && feof(scriptFile))
			break;
		//We have loaded a line...
		//Stage one, check the start char (switch it)
		char *copyofLine = &scriptLine[0];
		int workInt;
		switch (scriptLine[0])
		{
		case 0:
			break;
		case '=':
			//right, next char is 0->7 for output mode.
			//if we read 0, simply abort script as it makes no sense!
			myOutputMode = (scriptLine[1] - '0');
			if (myOutputMode == 0)
			{
				printf("Okay, read output mode of 0 (no output) so there is no point in continuing.\n");
				exit(10);
			}
			if (myOutputMode > 7)
				myOutputMode = 7;
			if (myVerbosityLevel > 3)
			{
				printf("Setting output mode to %d.\n",myOutputMode);
				if ((myOutputMode & 1) == 1)
					printf("Norn genetics will be written out.\n");
				if ((myOutputMode & 2) == 2)
					printf("Grendel genetics will be written out.\n");
				if ((myOutputMode & 4) == 4)
					printf("Ettin genetics will be written out.\n");
			}
			break;
		case '#':
			break;
		case '<':
			//prepare alias
			//Expected format is...
			// <nPATH
			//e.g.
			// <3c:\mooop\ okay?
			workInt = *(++copyofLine) - '0';
			if (verbosity > 7)
				printf("Building an alias...\n");
			buildAlias(workInt, ++copyofLine, false,verbosity);
			break;
		case '+':
		case '*':
			//prepare sourceline object

			//All we do here is instantiate a CSource object with the sourceline,
			//and assuming all goes well (exceptions get thrown if it doesn't) we add it
			//to our vector...
			mySources.push_back(new Source(copyofLine,verbosity));
			if (mySources.size() == 101)
			{
				printf("Too many permutation levels - go get a life!\n");
				exit(1);
			}
			break;
		case '>':
			//set destination folder
			myDumpFolder = expandFileRef(++copyofLine);
			if (myDumpFolder.at(myDumpFolder.length()-1) != PATHSEP)
				myDumpFolder.append(PATHSEPS);
			if (verbosity > 7)
				printf("Setting destination folder to %s\n",myDumpFolder.c_str());
			break;
		default:
			printf("Unknown command on line %s in file %s\n",scriptLine,filename);
			mySourceName = "***";
			fclose(scriptFile);
			return;
		}
	}

	mySourceName = filename;

	fclose(scriptFile);
}

// ----------------------------------------------------------------------------------
// Destructor
// Arguments:	(None)
// Returns:		(None)
// Description:	Cleans the whole caboodle up.
// ----------------------------------------------------------------------------------
ScriptExecutive::~ScriptExecutive()
{
	//Everything cleans itself up - aah the wonders of scope :)
}

// ----------------------------------------------------------------------------------
// Method:		Prepare
// Arguments:	(None)
// Returns:		True if preparation worked.
// Description:	Prepares all the sourcelines, thus genetics etc.
// ----------------------------------------------------------------------------------
bool ScriptExecutive::Prepare()
{
	//Prep all of the source lines
	std::vector<Source*>::iterator it;
	for(it = mySources.begin(); it != mySources.end(); it++ )
	{
		(*it)->Prepare();
	}
	return false;
}

// This attempts to get a version number from the genetics file's name.
int extractVersionNumber(std::string filename)
{
	//printf("*** %s",filename.c_str());
	std::string numdict = "0123456789";
	int num = 0;
	
	if (numdict.find(filename.at(0)) < 0)
		return -1;
	num = numdict.find(filename.at(0)) * 10;
	
	if (numdict.find(filename.at(1)) < 0)
		return -1;
	num += numdict.find(filename.at(1));
	//printf(" %d\n",num);
	return (num);
}

// ----------------------------------------------------------------------------------
// Method:		Execute
// Arguments:	(None)
// Returns:		(None)
// Description:	Performs the permutation operations.
// ----------------------------------------------------------------------------------
void ScriptExecutive::Execute()
{
	//The task here is to perform the execution of the permutations
	//to do this, we generate a Genome and perform transfers from each
	//of the genome sets in turn.


	int files[100];
	int i;

	for(i=0;i<mySources.size();i++)
		files[i] = -1;
	i=0;

	//Now we do the magic calculate the dictionaryGeneration number....
	if (dictionaryMode)
	{
		//set geneGeneration to the best generation known...
		//start with the first of the first as it were :)
		geneGeneration = extractVersionNumber(mySources.at(0)->myGenomes.at(0)->myNormalisedName);
		if (geneGeneration > -1)
			for(int src=0;src < mySources.size(); src++)
				for(int fil=0;fil < mySources.at(src)->myGenomes.size(); fil++)
				{
					int thisnumber = extractVersionNumber(mySources.at(src)->myGenomes.at(fil)->myNormalisedName);
					if (thisnumber < 0)
					{
						geneGeneration = -1;
						src = mySources.size();						
						break;
					}
					if (abs(thisnumber - geneGeneration) > generationGap)
						//Select Smaller
						geneGeneration = thisnumber<geneGeneration?thisnumber:geneGeneration;
					else
						//Select Larger
						geneGeneration = thisnumber>geneGeneration?thisnumber:geneGeneration;
				}
		if (geneGeneration == -1)
		{
			printf("***REVERTING TO LONG NAMES - INPUT NOT IN VERSIONED FORMAT***\n");
			dictionaryMode = 0;
		}
	}

	if (dictionaryMode)
		geneGeneration = (geneGeneration + 1) % 100;


	char generation[] = "00";
	if (dictionaryMode && (geneGeneration > 0))
	{
		sprintf(generation,"%02d",geneGeneration);
	}

	//printf("Doing permutations? %d %d\n",files[0],mySources.at(0)->myGenomes.size());

	//Which variant are we dealing with?
	int variant = 0;

	FILE* notesFile;

	std::string notes;
	notes.assign(myDumpFolder);
	notes.append("permnote.txt");

	if (!(notesFile = fopen(notes.c_str(),"a")))
	{
		printf("Failure when opening notes file for writing :(");
		exit(2);
	}

	time_t now;

	time(&now);

	char username[64];
	unsigned long space = 63;
	GetUserName(username,&space);

	//Write a timestamp and userstamp to the genetics notes file :)
	fprintf(notesFile,versionString());
	fprintf(notesFile,"Genetics Permutation set generated on %s by %s\n",ctime(&now),username);

	while ((files[0]) < (int)(mySources.at(0)->myGenomes.size()))
	{
		//At level i, increment and add level if within specs.
		//if outside specs, set to -1 and jump back, incrementing back.
		//if level i is past end of list, then perform permutation
		//printf("Permuting at level %d with entry %d\n",i,files[i]);
		if (i == mySources.size())
		{
			//perform permutation here

			//To make this permutation, create a genome and do the masks...

			Genetics* newGen;			

			for(int genus = 0; genus < 3; genus++)
			if ((myOutputMode & (1<<genus)) == (1<<genus))
			{
				newGen = new Genetics(genus,dictionaryMode==0?genusname[genus]:generation,myVerbosityLevel);
				for(int pos=0;pos<mySources.size();pos++)
				{
					//If we are in dictionary mode 0, merge according to name, otherwise pass ignorename 		
					newGen->mergeIn(mySources.at(pos)->myGenomes.at(files[pos]),mySources.at(pos)->mySearchMasks.at(0),!(mySources.at(pos)->myMerging),(dictionaryMode == 0));
					/*
					 * Now deal with modes otherwise.....
					 */
					
					if (dictionaryMode == 2)
						newGen->myNormalisedName.append(numberingDictionary.substr(files[pos],1));

				}
				if (dictionaryMode == 1)
					newGen->myNormalisedName.append(numberingDictionary.substr(variant,1));
				if (dictionaryMode)
					newGen->myNormalisedName.append(genusletters[genus]);

				fprintf(notesFile,"%s ->",newGen->myNormalisedName.c_str());
				for(pos=0;pos<mySources.size();pos++)
					fprintf(notesFile," %s",mySources.at(pos)->myGenomes.at(files[pos])->myGeneticsFile.c_str());
				fprintf(notesFile,"\n");

				if (dictionaryMode)
					newGen->myNormalisedName.append(".gen");	//As if not dictionary'ing, .gen already there :)
				
				newGen->blitOut(myDumpFolder.c_str());
			}
			variant++; // As we have done another variant in the permutation sequence :)
			//decrement i to return it into the permutation sequence...
			i--;
		}
		else
		{
			files[i]++;
			if(files[i] == mySources.at(i)->myGenomes.size())
			{
				i--;
			}
			else
			{
				i++;
				files[i] = -1;
			}
		}
	}
	fprintf(notesFile,"------\n");
	fclose(notesFile);
}



//EOF

}

