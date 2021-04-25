#include "fileformats/PrayFileReader.h"
#include "utils/endianlove.h"
#include "utils/optional.h"
#include "utils/span.h"
#include "utils/spanstream.h"

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

static bool is_printable(const std::string& s) {
	for (auto c : s) {
		if (static_cast<unsigned char>(c) < 32) {
			return false;
		}
	}
	return true;
}

optional<PrayTagBlock> get_block_as_tags(PrayFileReader& file, int i) {
	for (auto tagblock : tagblocks) {
		if (file.getBlockType(i) == tagblock) {
			return file.getBlockTags(i);
		}
	}
	if (file.getBlockType(i) == "FILE") {
		return {};
	}
	auto buf = file.getBlockRawData(i);
	// maybe it's a tag block anyways?
	spanstream s(buf);
	unsigned int nointvalues = read32le(s);
	if (s.fail()) {
		return {};
	}


	std::map<std::string, unsigned int> integerValues;
	for (unsigned int i = 0; i < nointvalues; i++) {
		unsigned int keylength = read32le(s);
		if (s.fail()) {
			return {};
		}
		if (keylength > 256) {
			return {};
		}

		std::string key(keylength, '0');
		s.read(&key[0], keylength);
		if (s.fail()) {
			return {};
		}
		if (!is_printable(key)) {
			return {};
		}

		unsigned int value = read32le(s);
		if (s.fail()) {
			return {};
		}

		integerValues[key] = value;
	}

	unsigned int nostrvalues = read32le(s);
	if (s.fail()) {
		return {};
	}

	std::map<std::string, std::string> stringValues;
	for (unsigned int i = 0; i < nostrvalues; i++) {
		unsigned int keylength = read32le(s);
		if (s.fail()) {
			return {};
		}

		std::string key(keylength, '0');
		s.read(&key[0], keylength);
		if (s.fail()) {
			return {};
		}
		if (!is_printable(key)) {
			return {};
		}

		unsigned int valuelength = read32le(s);
		if (s.fail()) {
			return {};
		}

		std::string value(valuelength, '0');
		s.read(&value[0], valuelength);
		if (s.fail()) {
			return {};
		}
		if (!is_printable(value)) {
			return {};
		}

		stringValues[key] = value;
	}
	s.peek();
	if (!s.eof()) {
		return {};
	}
	cerr << "Unknown block type '" << file.getBlockType(i) << "' looks like a tag block" << endl;
	return PrayTagBlock(integerValues, stringValues);
}

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

		auto tags = get_block_as_tags(file, i);
		if (tags) {
			pray_source << endl
						<< "group " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\"" << endl;

			auto int_tags = tags->first;
			auto string_tags = tags->second;

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
		} else {
			pray_source << endl
						<< "inline " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\" \"" << file.getBlockName(i) << "\"" << endl;
			cout << "Writing " << (output_directory / file.getBlockName(i)) << endl;
			ofstream output(output_directory / file.getBlockName(i));
			auto buf = file.getBlockRawData(i);
			output.write((char*)buf.data(), buf.size());
		}
	}
}
