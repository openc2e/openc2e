#include "prayfile/pray.h"
#include <iostream>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <array>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;

const std::array<std::string, 11> tagblocks = {
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
	if (argc != 2) {
		cerr << "syntax: praydumper filename" << endl;
		return 1;
	}

	fs::path inputfile = fs::path(argv[1]);
	if (!fs::exists(inputfile)) {
		cerr << "input file doesn't exist!" << endl;
		return 1;
	}

	fs::path output_directory = inputfile.stem();
	if (fs::exists(output_directory)) {
		cerr << "Output directory " << output_directory << " already exists" << endl;
		exit(1);
	}
	if (!fs::create_directory(output_directory)) {
		cerr << "Couldn't create output directory " << output_directory << endl;
		exit(1);
	}

	std::string pray_source_filename = (output_directory / inputfile.stem()).string() + ".txt";
	std::ofstream pray_source(pray_source_filename);
	cout << "Writing \"" << pray_source_filename << "\"" << endl;
	pray_source << "(- praydumper-generated PRAY file from '" << argv[0] << "' -)" << endl;
	pray_source << endl << "\"en-GB\"" << endl;

	prayFile file(inputfile);

	for (vector<prayFileBlock *>::iterator x = file.blocks.begin(); x != file.blocks.end(); x++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)
		
		(*x)->load();
		
		bool handled = false;
		for (auto tagblock : tagblocks) {
			if ((*x)->type == tagblock) {
				handled = true;
				pray_source << endl << "group " << (*x)->type << " \"" << (*x)->name << "\"" << endl;

				(*x)->parseTags();
				
				for (std::map<std::string, int>::iterator y = (*x)->integerValues.begin(); y != (*x)->integerValues.end(); y++) {
					pray_source << "\"" << y->first << "\" " << y->second << endl;
				}
				
				for (std::map<std::string, std::string>::iterator y = (*x)->stringValues.begin(); y != (*x)->stringValues.end(); y++) {
					std::string name = y->first;
					if ((name.substr(0, 7) ==  "Script ") || (name.substr(0, 13) == "Remove script")) {
						name = (*x)->name + " - " + name + ".cos";
						cout << "Writing " << (output_directory / name) << endl;
						ofstream output(output_directory / name);
						output.write(y->second.c_str(), y->second.size());
						pray_source << "\"" << y->first << "\" @ \"" << name << "\"" << endl;
					} else {
						pray_source << "\"" << y->first << "\" \"" << y->second << "\"" << endl;
					}
				}
			}
		}
		
		if (!handled) {
			pray_source << endl << "inline " << (*x)->type << " \"" << (*x)->name << "\" \"" << (*x)->name << "\"" << endl;
			cout << "Writing " << (output_directory / (*x)->name) << endl;
			ofstream output(output_directory / (*x)->name);
			output.write((char *)(*x)->getBuffer(), (*x)->getSize());
		}
	}
}

