#include "common/endianlove.h"
#include "common/optional.h"
#include "common/span.h"
#include "common/spanstream.h"
#include "fileformats/PrayFileReader.h"

#include <array>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fstream>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

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
	fmt::print(stderr, "Unknown block type '{}' looks like a tag block\n", file.getBlockType(i));
	return PrayTagBlock(integerValues, stringValues);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "syntax: praydumper filename\n");
		return 1;
	}

	fs::path inputfile = fs::path(argv[1]);
	if (!fs::exists(inputfile)) {
		fmt::print(stderr, "input file doesn't exist!");
		return 1;
	}

	fs::path output_directory = inputfile.stem();
	if (fs::exists(output_directory)) {
		fmt::print(stderr, "Output directory {} already exists\n", output_directory.string());
		exit(1);
	}
	if (!fs::create_directory(output_directory)) {
		fmt::print(stderr, "Couldn't create output directory {}\n", output_directory.string());
		exit(1);
	}

	std::string pray_source_filename = (output_directory / inputfile.stem()).string() + ".txt";
	std::ofstream pray_source(pray_source_filename);
	fmt::print("Writing {:?}\n", pray_source_filename);
	fmt::print(pray_source, "(- praydumper-generated PRAY file from {:?} -)\n", inputfile.filename().string());
	fmt::print(pray_source, "\n");
	fmt::print(pray_source, "\"en-GB\"\n");

	std::ifstream in(inputfile.string(), std::ios::binary);
	if (!in) {
		fmt::print(stderr, "Error opening file {}\n", inputfile.string());
		exit(1);
	}

	PrayFileReader file(in);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)

		auto tags = get_block_as_tags(file, i);
		if (tags) {
			fmt::print(pray_source, "\n");
			fmt::print(pray_source, "group {} \"{}\"\n", file.getBlockType(i), file.getBlockName(i));

			auto int_tags = tags->first;
			auto string_tags = tags->second;

			for (auto y : int_tags) {
				fmt::print(pray_source, "\"{}\" {}\n", y.first, y.second);
			}

			for (auto y : string_tags) {
				std::string name = y.first;
				if ((name.substr(0, 7) == "Script ") || (name.substr(0, 13) == "Remove script")) {
					name = file.getBlockName(i) + " - " + name + ".cos";
					fmt::print("Writing {}\n", (output_directory / name).string());
					ofstream output(output_directory / name);
					output.write(y.second.c_str(), y.second.size());
					fmt::print(pray_source, "\"{}\" @ \"{}\"\n", y.first, name);
				} else {
					fmt::print(pray_source, "\"{}\" \"{}\"\n", y.first, y.second);
				}
			}
		} else {
			fmt::print(pray_source, "\n");
			fmt::print(pray_source, "inline {} \"{}\" \"{}\"\n",
				file.getBlockType(i), file.getBlockName(i), file.getBlockName(i));

			fmt::print("Writing {}\n", (output_directory / file.getBlockName(i)).string());
			ofstream output(output_directory / file.getBlockName(i));
			auto buf = file.getBlockRawData(i);
			output.write((char*)buf.data(), buf.size());
		}
	}
}
