// PowerGEN.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "PowerGen.h"
#include "ScriptExecutive.h"
#pragma once

extern int dictionaryMode = 0;

void version()
{
	//Displays version number...
	printf(versionString());
}

char versionArray[] = "NextGEN - Genetic Splicer (c) Cyberlife 1999+ - 1.1.5\n";
char* versionString()
{
	return versionArray;
}

void usage()
{
	version();
	printf("Usage...\n");
	printf("nextgen <option> <file>\n");
	printf("\nAny number of -v's are allowed\n");
	printf("    -d sets numbering mode to one char\n");							//I.E. Dictionary mode == 1;
	printf("    -D sets numbering mode to one char per permutation level\n");	//I.E. Dictionary mode == 2;
	printf("    -r replaces genes with random data\n");
	printf("    -o randomly reorders genes\n");
	printf("\"nextgen --language\" gives a language synposis\n");
}

void language()
{
	usage();
	printf("Language...\n");
	printf("All commands start in column 1 - there is no leeway here\n");
	printf("#<comment>\n");
	printf("<nAlias e.g. <9c:\\windows\\desktop\n");
	printf(">folder e.g. >c:\\destinationgens\n");
	printf("+<GENESPEC>,<FILESPEC>\n");
	printf("*<GENESPEC>,<FILESPEC>\n");
	printf("+ is merge, * replace\n");
	printf("=n sets genus types to generate. Bitfield where 1=norn, 2=grendel, 4=ettin\n");
	printf("   defaults to 1 (norn only) (legal settings 1-7)\n");
	printf("[ENTER]\n");
	
	getchar();
	printf("Note that if you use * and there is no matching genes in the genespec then\n");
	printf("there will be no matching genes in the resultang gen file. Use * with care.\n\n");
	printf("Gene specs are 6 numbers as follows (255 is a wildcard to mean any)\n");
	printf("  Variant GeneGroup GeneType GeneID SubType SubID\n\n");
	printf("Variants are 0 thru 254\n");
	printf("Gene groups are 0=BRAIN, 1=BIOCHEM, 2=APPEARANCE ETC,\n     3=GAITS AND POSES, 4=BEHAVIOUR\n");
	printf("Gene Types depend on Gene group, and are as follows\n");
	printf("    BRAIN: 0=Organs, 1=Lobes, 2=Tracts\n");
	printf("  BIOCHEM: 0=Organs, 1=Initial Concentrations, 2=Half Life\n");
	printf(" CREATURE: 0=Appearance, 1=Pigments, 2=Pigment Bleeds, 3=Facial Expressions\n");
	printf(" GAITPOSE: 0=Gaits, 1=Poses\n");
	printf("BEHAVIOUR: 0=Neurotransmitters, 1=Instincts, 2=Stimuli\n[ENTER]\n");

	getchar();
	printf("\nGeneID is the ID of the gene in the GEN file\n\n");
	printf("The SubType and SubID are always 255 255 unless the genes are \n");
	printf("in the BIOCHEM/ORGAN group, and are reactions, emitters or receptors.\n In this case, the subtype is:\n");
	printf(" 0=Reactions, 1=Emitters, 2=Receptors.\nAnd the SubID the ID of the gene in the GEN file.\n");
	printf("For reactions, emitters and receptors, GeneType and GeneID refer to the organ\n");
	printf("which owns the gene - I.E. which organ the gene attaches to.\n[ENTER]\n");
	
	getchar();
	printf("\nYou can also put a !n at the end of the filespec to indicate a coerce action.\n");
	printf("There can only be one coerce per sourceline (at the end of the filespec)\n");
	printf("In a coerce, n is 0-9 for the variant. All variant 0 genes are set to variant n.\n");
	printf("     e.g. *255 255 255 255 255 255,test.gen!1 means select all from test.gen,\n");
	printf("                                              forcing the variant to 1\n");
	printf("\n\nTo use an alias in a filespec, use $n where n is 0-9 (the alias number)\n");
	printf("For your convenience, $0 is set to current folder, and $1 to the folder of the\n");
	printf("script during the initialisation of the process.\n");
	printf("\nNextGEN is (c)1999+ CyberLife Technology Ltd,\n      And is maintained by D.Silverstone\n");

}

void error()
{

	printf("Error on commandline\n");
	usage();
}

void dofile(char* filename, int verbosity)
{
	//printf("Processing %s at verbosity level %d\n",filename,verbosity);
#ifndef _DEBUG
	try
#endif
	{
		NextGEN::ScriptExecutive* sc;
		sc = new NextGEN::ScriptExecutive(filename,verbosity);
		sc->Prepare();
		sc->Execute();
		delete sc;
	}
#ifndef _DEBUG	
	catch ( ... )
	{
		printf("Exception caught!\n");
	}
#endif
}

int main(int argc, char* argv[])
{
	//NextGEN is a good utility.
	//0 params, or "--help" displays Usage then terminates
	// "--language" displays script language guide then terminates
	// "--version" displays version number then terminates
	// "-v" increments verbosity by one peg. (applicable to files after it on the commandline only)
	// All other params will be treated as script files.

	int verbositycount = 0;
	if (argc == 1)
	{
		usage();
		return 0;
	}
	if (argc == 2)
	{
		if (strcmp(argv[1],"--help") == 0)
		{
			usage();
		}
		else if (strcmp(argv[1],"--usage") == 0)
		{
			usage();
		}
		else if (strcmp(argv[1],"--language") == 0)
		{
			language();
		}
		else if (strcmp(argv[1],"--version") == 0)
		{
			version();
		}
		else
			dofile(argv[1],0);
		return 0;
	}

	for(int i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"-v"))
			verbositycount++;
		else if (!strcmp(argv[i],"-d"))
			dictionaryMode = 1;
		else if (!strcmp(argv[i],"-D"))
			dictionaryMode = 2;
		else if (!strcmp(argv[i],"-o")) {
			printf("\n[WARNING WARNING WARNING:  Producing randomly ordered GEN files.]\n");
			srand(time(NULL));
			theRandomlyReorderFlag = true;
		}
		else if (!strcmp(argv[i],"-r")) {

			printf("\n[WARNING WARNING WARNING:  Producing random content GEN files.]\n");
			srand(time(NULL));
			theReplaceByRandomDataFlag = true;
		} else
			dofile(argv[i],verbositycount);
	}
	return 0;
}

