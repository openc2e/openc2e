#include "endianlove.h"
#include "fileformats/mngfile.h"
#include "utils/enumerate.h"

#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <fstream>

namespace fs = ghc::filesystem;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "syntax: mngdumper filename" << std::endl;
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		std::cerr << "File " << input_path << " doesn't exist" << std::endl;
		exit(1);
	}

	fs::path stem = input_path.stem();
	fs::path output_directory = stem;

	if (!fs::create_directories(output_directory)) {
		if (!fs::is_directory(output_directory)) {
			std::cerr << "Couldn't create output directory " << output_directory << std::endl;
			exit(1);
		}
	}

	MNGFile file(argv[1]);

	fs::path script_filename((output_directory / stem).native() + ".txt");
	fmt::print("{}\n", script_filename.string());
	std::ofstream script(script_filename, std::ios_base::binary);
	script << file.script;

	for (auto sample : enumerate(file.samples)) {
		for (auto kv : file.samplemappings) {
			if (kv.second != sample.i) {
				continue;
			}
			fs::path sample_filename((output_directory / kv.first).native() + ".wav");
			fmt::print("{}\n", sample_filename.string());

			std::ofstream out((output_directory / kv.first).native() + ".wav", std::ios_base::binary);
			out.write("RIFF", 4);
			write32le(out, 4 + sample->second); // TODO: RIFF chunk size
			out.write("WAVE", 4);
			out.write("fmt ", 4);
			out.write(sample->first, sample->second);
		}
	}
}