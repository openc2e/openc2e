#include "agentfile.h"
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
	cerr << "praydumper, part of openc2e, copyright (c) 2004 Alyssa Milburn" << endl << endl;
	if (argc != 2) {
		cerr << "syntax: praydumper filename" << endl;
		return 1;
	}
	ifstream input(argv[1]);
	// todo: check input is valid
	cout << "(- praydumper-generated PRAY file from '" << argv[1] << "' -)" << endl;
	cout << endl << "\"en-GB\"" << endl;
	prayFile file;
	input >> file;
	cerr << "read PRAY file okay" << endl;

	for (vector<block *>::iterator x = file.blocks.begin(); x != file.blocks.end(); x++) {
		tagPrayBlock *b = dynamic_cast<tagPrayBlock *>(*x);

		if (b) {
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

