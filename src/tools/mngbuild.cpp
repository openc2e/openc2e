#include "common/endianlove.h"
#include "common/readfile.h"
#include "fileformats/mngfile.h"
#include "fileformats/mngparser.h"

#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "syntax: mngdumper scriptfile\n");
		exit(1);
	}

	std::string script_path = argv[1];
	if (!fs::exists(script_path)) {
		fmt::print(stderr, "File {} doesn't exist\n", script_path);
		exit(1);
	}

	auto script = readfile(script_path);
	auto sample_names = mngparse(script).getWaveNames();
	for (auto& s : sample_names) {
		// TODO: case-insensitive
		s = (fs::path(script_path).parent_path() / s).string() + ".wav";
	}

	std::string output_path(fs::path(script_path).stem().string() + ".mng");
	fmt::print("Writing to {}\n", output_path);
	std::ofstream out(output_path, std::ios_base::binary);

	fmt::print("Writing header...\n");
	const int number_of_samples = sample_names.size();
	fmt::print("number of samples = {}\n", number_of_samples);
	write32le(out, number_of_samples);

	const int script_position = 12 + number_of_samples * 8;
	write32le(out, script_position);

	const int script_size = script.size();
	fmt::print("script size = {}\n", script_size);
	write32le(out, script_size);

	int sample_position = script_position + script_size;
	for (auto s : sample_names) {
		write32le(out, sample_position);
		// TODO: make sure it's actually a wave
		// TODO: does it have to be specifically the fmt chunk and then the data, or can other chunks exist?
		// TODO: does it have to be 16-bit 22kHz mono audio?
		// TODO: better error if it doesn't exist
		const int sample_size = fs::file_size(s) - 16;
		write32le(out, sample_size);
		sample_position += sample_size;
	}

	fmt::print("Writing script...\n");
	auto scrambled_script = mngencrypt(script);
	out.write((char*)scrambled_script.data(), scrambled_script.size());

	fmt::print("Writing samples...\n");
	for (auto s : sample_names) {
		fmt::print("{}\n", s);
		auto sample = readfilebinary(s);
		// TODO: check size matches what we wrote earlier
		out.write((char*)sample.data() + 16, sample.size() - 16);
	}

	fmt::print("Done!\n");

	return 0;
}