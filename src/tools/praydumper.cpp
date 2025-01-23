#include "common/endianlove.h"
#include "common/io/FileReader.h"
#include "common/io/FileWriter.h"
#include "common/io/IOException.h"
#include "common/io/SpanReader.h"
#include "common/io/WriterFmt.h"
#include "common/optional.h"
#include "common/span.h"
#include "fileformats/PrayFileReader.h"

#include <array>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

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

optional<PrayTagBlock> maybe_get_block_as_tags(PrayFileReader& file, int i) {
	for (auto tagblock : tagblocks) {
		if (file.getBlockType(i) == tagblock) {
			return file.getBlockTags(i);
		}
	}
	if (file.getBlockType(i) == "FILE") {
		return {};
	}

	// maybe it's a tag block anyways?
	try {
		auto tags = file.getBlockTags(i);
		for (const auto& int_tag : tags.first) {
			if (!is_printable(int_tag.first)) {
				return {};
			}
		}
		for (const auto& string_tag : tags.second) {
			if (!is_printable(string_tag.first) || !is_printable(string_tag.second)) {
				return {};
			}
		}
		fmt::print(stderr, "Successfully parsed unknown block type '{}' as a tag block\n", file.getBlockType(i));
		return tags;
	} catch (...) {
		return {};
	}
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
	FileWriter pray_source(pray_source_filename);
	fmt::print("Writing {:?}\n", pray_source_filename);
	fmt::print(pray_source, "(- praydumper-generated PRAY file from {:?} -)\n", inputfile.filename().string());
	fmt::print(pray_source, "\n");
	fmt::print(pray_source, "\"en-GB\"\n");

	FileReader in(inputfile);
	PrayFileReader file(in);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)

		auto tags = maybe_get_block_as_tags(file, i);
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
					FileWriter output(output_directory / name);
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
			FileWriter output(output_directory / file.getBlockName(i));
			auto buf = file.getBlockRawData(i);
			output.write((char*)buf.data(), buf.size());
		}
	}
}
