#include "pray.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <boost/filesystem/convenience.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;

#define NO_TAGBLOCKS 11
const char *tagblocks[NO_TAGBLOCKS] = {
	"AGNT", // C3 agent
	"DSAG", // DS agent
	"MACH", // SM agent
	"HAND", // SM agent
	"LIVE", // SM agent
	"MONK", // SM agent
	"EXPC", // C3 creature info
	"DSEX", // DS creature info
	"SFAM", // C3 starter family
	"EGGS", // eggs
	"DFAM" // DS starter family
};

int main(int argc, char **argv) {
	bool outputfiles = true, notags = false, usageerror = false;
	int ch;
	while ((ch = getopt(argc, argv, "on")) != -1) {
		switch (ch) {
			case 'o': outputfiles = false; break;
			case 'n': notags = true; break;
			default: usageerror = true; break;
		}
	}
	argc -= optind;
	argv += optind;
	if ((argc != 1) || usageerror) {
		cerr << "syntax: praydumper -o -n filename\n-o don't output non-tag blocks as files\n-n do not interpret tag blocks" << endl;
		return 1;
	}

	fs::path inputfile = fs::path(argv[0], fs::native);
	if (!fs::exists(inputfile)) {
		cerr << "input file doesn't exist!" << endl;
		return 1;
	}

	cout << "(- praydumper-generated PRAY file from '" << argv[0] << "' -)" << endl;
	cout << endl << "\"en-GB\"" << endl;
	prayFile file(inputfile);

	for (vector<prayBlock *>::iterator x = file.blocks.begin(); x != file.blocks.end(); x++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)
		
		(*x)->load();
		
		bool handled = false;
		for (unsigned int i = 0; i < NO_TAGBLOCKS; i++) {
			if ((*x)->type == tagblocks[i]) {
				handled = true;
				cout << endl << "group " << (*x)->type << " \"" << (*x)->name << "\"" << endl;

				(*x)->parseTags();
				
				for (std::map<std::string, int>::iterator y = (*x)->integerValues.begin(); y != (*x)->integerValues.end(); y++) {
					cout << "\"" << y->first << "\" " << y->second << endl;
				}
				
				for (std::map<std::string, std::string>::iterator y = (*x)->stringValues.begin(); y != (*x)->stringValues.end(); y++) {
					std::string name = y->first;
					if ((name.substr(0, 7) ==  "Script ") || (name.substr(0, 13) == "Remove script")) {
						name = (*x)->name + " - " + name + ".cos";
						ofstream output(name.c_str());
						output.write(y->second.c_str(), y->second.size());
						cout << "\"" << y->first << "\" @ \"" << name << "\"" << endl;
					} else {
						cout << "\"" << y->first << "\" \"" << y->second << "\"" << endl;
					}
				}
			}
		}
		
		if (!handled) {
			cout << endl << "inline " << (*x)->type << " \"" << (*x)->name << "\" \"" << (*x)->name << "\"" << endl;	

			if (outputfiles) {
				ofstream output((*x)->name.c_str());
				output.write((char *)(*x)->getBuffer(), (*x)->getSize());	
			}
		}
	}
}

