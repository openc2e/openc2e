#include "agentfile.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

int main(int argc, char **argv) {
	bool outputfiles = false, notags = false, usageerror = false;
	cerr << "praydumper, part of openc2e, copyright (c) 2004 Alyssa Milburn" << endl << endl;
	int ch;
	while ((ch = getopt(argc, argv, "on")) != -1) {
		switch (ch) {
			case 'o': outputfiles = true; break;
			case 'n': notags = true; break;
			default: usageerror = true; break;
		}
	}
	argc -= optind;
	argv += optind;
	if ((argc != 1) || usageerror) {
		cerr << "syntax: praydumper -o -n filename\n-o output uninterpreted blocks as files (currently unimplemented)\n-n do not interpret tag blocks" << endl;
		return 1;
	}
	ifstream input(argv[0]);
	// todo: check input is valid
	cout << "(- praydumper-generated PRAY file from '" << argv[0] << "' -)" << endl;
	cout << endl << "\"en-GB\"" << endl;
	prayFile file;
	input >> file;
	cerr << "read PRAY file okay" << endl;

	for (vector<block *>::iterator x = file.blocks.begin(); x != file.blocks.end(); x++) {
		tagPrayBlock *b = dynamic_cast<tagPrayBlock *>(*x);

		if ((!notags) && b) {
			cout << endl << "group " << (*x)->blockname() << " \"" << (*x)->name() << "\"" << endl;
			for (vector<pair<string, unsigned int> >::iterator y = b->intvalues.begin(); y != b->intvalues.end(); y++) {
				cout << "\"" << y->first << "\" " << y->second << endl;
			}
			for (vector<pair<string, string> >::iterator y = b->strvalues.begin(); y != b->strvalues.end(); y++) {
				// todo: s/"/\\"/ in the data (use find/replace methods of string)
				cout << "\"" << y->first << "\" \"" << y->second << "\"" << endl;
			}
		} else {
			cout << endl << "inline " << (*x)->blockname() << " \"" << (*x)->name() << "\" \"" << (*x)->name() << "\"" << endl;	
		}
	}
}

