#include <iostream>
#include <fstream>
#include <string>

#include "../../common/MapScanner.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Sorry, you must drag a map file onto the shortcut" << std::endl;
		return 1;
	}
	std::cout << "Using map: " << argv[1] << std::endl;
	std::cout << "Please enter error offset in full (e.g. 0045827): ";
	unsigned int ofs;
	std::cin >> std::hex >> ofs;

	std::cout << "Please wait, finding..." << std::endl;

	MapScanner mapping(argv[1],ofs);

	std::cout << mapping.functionName << std::endl << mapping.objectFile << std::endl;

	std::cin >> ofs;

	return 0;
	
}

