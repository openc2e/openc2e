#include "mngfile.h"
#include <fstream>
#include "mngparser.tab.hpp"

extern int mngparse();

int main(int argc, char **argv) {
	if (argc != 2) return 1;

	std::ifstream in(argv[1]);
	mngrestart(&in);
	
	mngparse();	
}

