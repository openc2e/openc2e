#include "endianlove.h"
#include "mngfile/mngfile.h"


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

	for (auto kv : file.samplemappings) {
		fs::path sample_filename((output_directory / kv.first).native() + ".wav");
		fmt::print("{}\n", sample_filename.string());
		std::ofstream sample((output_directory / kv.first).native() + ".wav", std::ios_base::binary);

		sample.write("RIFF", 4);
		write32le(sample, 36 + file.samples[kv.second].second); // TODO: RIFF chunk size
		sample.write("WAVE", 4);
		sample.write("fmt ", 4);
		write32le(sample, 16); // fmt chunk size
		write16le(sample, 0x0001); // WAVE_FORMAT_PCM
		write16le(sample, 1); // num channels
		write32le(sample, 22050); // samples per sec
		write32le(sample, 22050 * 2); // bytes per sec
		write16le(sample, 2); // block align
		write16le(sample, 8 * 2); // bits per sample
		sample.write("data", 4);
		write32le(sample, file.samples[kv.second].second); // data chunk size
		sample.write(file.samples[kv.second].first, file.samples[kv.second].second);
	}
}