#include "fileformats/PrayFileWriter.h"
#include "fileformats/PraySourceParser.h"

#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <map>
#include <string>

namespace fs = ghc::filesystem;

int main(int argc, char** argv) {
	{
		if (!(argc == 2 || argc == 3)) {
			printf("USAGE: %s INPUT [OUTPUT]\n", argv[0]);
			return 1;
		}

		std::ifstream f(argv[1]);
		std::string str((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());

		fs::path parent_path = fs::path(argv[1]).parent_path();

		std::string output_filename;
		if (argc == 3) {
			output_filename = argv[2];
		} else {
			output_filename = fs::path(argv[1]).stem().string() + ".agents";
		}

		auto events = PraySourceParser::parse(str);
		if (auto* error = events[0].get_if<PraySourceParser::Error>()) {
			std::cout << "Error: " << error->message << "\n";
			return 1;
		}

		std::cout << "Writing output to \"" << output_filename << "\"" << std::endl;
		std::ofstream out(output_filename, std::ios::binary);
		if (!out) {
			std::cerr << "Couldn't open \"" << output_filename << "\"" << std::endl;
			exit(1);
		}
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
				std::cout << "Tag block " << event->type << " \""
						  << event->name << "\"\n";

			} else if (auto* event = res.get_if<PraySourceParser::InlineBlock>()) {
				std::cout << "Inline block " << event->type << " \""
						  << event->name << "\" from file \"" << event->filename
						  << "\"\n";

				// TODO: check in same directory
				std::ifstream in((parent_path / event->filename).string());
				if (!in) {
					std::cout << "Couldn't open file \""
							  << (parent_path / event->filename).string() << "\""
							  << std::endl;
					exit(1);
				}
				std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)),
					std::istreambuf_iterator<char>());

				writer.writeBlockRawData(event->type, event->name, data);

			} else if (auto* event = res.get_if<PraySourceParser::StringTag>()) {
				string_tags[event->key] = event->value;

			} else if (auto* event = res.get_if<PraySourceParser::StringTagFromFile>()) {
				// TODO: check in same directory
				std::ifstream in((parent_path / event->filename).string());
				if (!in) {
					std::cout << "Couldn't open file \""
							  << (parent_path / event->filename).string() << "\""
							  << std::endl;
					exit(1);
				}
				std::string val((std::istreambuf_iterator<char>(in)),
					std::istreambuf_iterator<char>());

				string_tags[event->key] = val;

			} else if (auto* event = res.get_if<PraySourceParser::IntegerTag>()) {
				int_tags[event->key] = event->value;

			} else {
				std::cout << "Not Implemented: " << format_as(res) << std::endl;
			}
		}

		std::cout << "Done!" << std::endl;
	}
}
