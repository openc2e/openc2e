#include "common/Ranges.h"
#include "common/endianlove.h"
#include "fileformats/mngfile.h"

#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <utility>

namespace fs = ghc::filesystem;

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "syntax: mngdumper filename\n");
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		fmt::print(stderr, "File {:?} doesn't exist\n", input_path.string());
		exit(1);
	}

	fs::path stem = input_path.stem();
	fs::path output_directory = stem;

	if (!fs::create_directories(output_directory)) {
		if (!fs::is_directory(output_directory)) {
			fmt::print(stderr, "Couldn't create output directory {:?}\n", output_directory.string());
			exit(1);
		}
	}

	MNGFile file(argv[1]);

	fs::path script_filename((output_directory / stem).native() + ".txt");
	fmt::print("{}\n", script_filename.string());
	std::ofstream script(script_filename, std::ios_base::binary);
	script.write(file.script.c_str(), file.script.size());

	for (auto kv : zip(file.getSampleNames(), file.samples)) {
		fs::path sample_filename((output_directory / kv.first).native() + ".wav");
		fmt::print("{}\n", sample_filename.string());

		std::ofstream out((output_directory / kv.first).native() + ".wav", std::ios_base::binary);
		out.write((const char*)kv.second.data(), kv.second.size());
	}
}