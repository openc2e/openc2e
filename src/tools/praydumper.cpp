#include "fileformats/PrayFileReader.h"

#include <array>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>

namespace fs = ghc::filesystem;

using std::cerr;
using std::cout;
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

int main(int argc, char** argv) {
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
	pray_source << "(- praydumper-generated PRAY file from '" << inputfile.filename().string() << "' -)" << endl;
	pray_source << endl
				<< "\"en-GB\"" << endl;

	std::ifstream in(inputfile.string(), std::ios::binary);
	if (!in) {
		cerr << "Error opening file " << inputfile << endl;
		exit(1);
	}

	PrayFileReader file(in);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)

		bool handled = false;
		for (auto tagblock : tagblocks) {
			if (file.getBlockType(i) == tagblock) {
				handled = true;
				pray_source << endl
							<< "group " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\"" << endl;

				auto tags = file.getBlockTags(i);
				auto int_tags = tags.first;
				auto string_tags = tags.second;

				for (auto y : int_tags) {
					pray_source << "\"" << y.first << "\" " << y.second << endl;
				}

				for (auto y : string_tags) {
					std::string name = y.first;
					if ((name.substr(0, 7) == "Script ") || (name.substr(0, 13) == "Remove script")) {
						name = file.getBlockName(i) + " - " + name + ".cos";
						cout << "Writing " << (output_directory / name) << endl;
						ofstream output(output_directory / name);
						output.write(y.second.c_str(), y.second.size());
						pray_source << "\"" << y.first << "\" @ \"" << name << "\"" << endl;
					} else {
						pray_source << "\"" << y.first << "\" \"" << y.second << "\"" << endl;
					}
				}
			}
		}

		if (!handled) {
			pray_source << endl
						<< "inline " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\" \"" << file.getBlockName(i) << "\"" << endl;
			cout << "Writing " << (output_directory / file.getBlockName(i)) << endl;
			ofstream output(output_directory / file.getBlockName(i));
			auto buf = file.getBlockRawData(i);
			output.write((char*)buf.data(), buf.size());
		}
	}
}
