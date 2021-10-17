// Catalogue Checker.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
	#include "stdafx.h"
#endif
#include "../../common/Catalogue.h"
#include <iostream>
#include <string>
int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			std::cerr << "Specify language id and directories (starting with main, then auxiliary) as the command line options" << std::endl;
			return 1;
		}

		Catalogue catalogue;
		// Add each auxiliary directories
		for (int i = 2; i < argc; ++i)
		{
			Catalogue localCatalogue;
			localCatalogue.AddDir(argv[i], argv[1]);
			catalogue.Merge(localCatalogue);			
		}

		catalogue.DumpTags(std::cout);
	}
	catch (Catalogue::Err e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

