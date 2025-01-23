#include "common/io/FileReader.h"
#include "common/io/FileWriter.h"
#include "common/readfile.h"
#include "fileformats/PrayFileWriter.h"
#include "fileformats/PraySourceParser.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>
#include <map>
#include <string>

namespace fs = ghc::filesystem;

int main(int argc, char** argv) {
	{
		if (!(argc == 2 || argc == 3)) {
			printf("USAGE: %s INPUT [OUTPUT]\n", argv[0]);
			return 1;
		}

		std::string str = readfile(argv[1]);

		fs::path parent_path = fs::path(argv[1]).parent_path();

		std::string output_filename;
		if (argc == 3) {
			output_filename = argv[2];
		} else {
			output_filename = fs::path(argv[1]).stem().string() + ".agents";
		}

		auto events = PraySourceParser::parse(str);
		if (auto* error = events[0].get_if<PraySourceParser::Error>()) {
			fmt::print(stderr, "Error: {}\n", error->message);
			return 1;
		}

		fmt::print("Writing output to {:?}\n", output_filename);
		FileWriter out(output_filename);
		PrayFileWriter writer(out);

		std::map<std::string, std::string> string_tags;
		std::map<std::string, unsigned int> int_tags;

		for (auto res : events) {
			if (res.has<PraySourceParser::Error>()) {
				// handled already

			} else if (res.has<PraySourceParser::GroupBlockStart>()) {
				string_tags = {};
				int_tags = {};

			} else if (auto* event = res.get_if<PraySourceParser::GroupBlockEnd>()) {
				writer.writeBlockTags(event->type, event->name, int_tags, string_tags);
				fmt::print("Tag block {} \"{}\"\n", event->type, event->name);

			} else if (auto* event = res.get_if<PraySourceParser::InlineBlock>()) {
				fmt::print("Inline block {} \"{}\" from file {:?}\n", event->type, event->name, event->filename);

				// TODO: check in same directory
				FileReader in(parent_path / event->filename);
				auto data = in.read_to_end();
				writer.writeBlockRawData(event->type, event->name, data);

			} else if (auto* event = res.get_if<PraySourceParser::StringTag>()) {
				string_tags[event->key] = event->value;

			} else if (auto* event = res.get_if<PraySourceParser::StringTagFromFile>()) {
				// TODO: check in same directory
				FileReader in(parent_path / event->filename);
				std::string val = readfile(in);
				string_tags[event->key] = val;

			} else if (auto* event = res.get_if<PraySourceParser::IntegerTag>()) {
				int_tags[event->key] = event->value;

			} else {
				fmt::print(stderr, "Not Implemented: {}\n", res);
			}
		}

		fmt::print("Done!\n");
	}
}
