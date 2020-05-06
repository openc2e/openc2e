#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <zlib.h>

#include "endianlove.h"
#include "spanstream.h"

namespace fs = ghc::filesystem;

static const std::string CREATURES_ARCHIVE_MAGIC = "Creatures Evolution Engine - Archived information file. zLib 1.13 compressed";

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "syntax: creaturesarchivedumper filename" << std::endl;
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		std::cerr << "File " << input_path << " doesn't exist" << std::endl;
		exit(1);
	}

	fs::path stem = input_path.stem();

	std::ifstream in(input_path, std::ios::binary);
	std::vector<uint8_t> data(std::istreambuf_iterator<char>{in}, {});
	fmt::print("data size = {}\n", data.size());

	spanstream s(data);
	std::string magic;
	magic.resize(CREATURES_ARCHIVE_MAGIC.size());
	s.read(&magic[0], magic.size());


	if (magic != CREATURES_ARCHIVE_MAGIC) {
		fmt::print(stderr, "Invalid magic - got \"{}\"\n", magic);
		exit(1);
	}

	fmt::print("magic = \"{}\"\n", magic);
	fmt::print("magic size = {}\n", magic.size());

	uint8_t unknown1 = read8(s);
	uint8_t unknown2 = read8(s);
	uint8_t unknown3 = read8(s);
	fmt::print("unknown1 = 0x{:02x} ({} << 8 = {})\n", unknown1, unknown1, unknown1 << 8);
	fmt::print("unknown2 = 0x{:02x} ({} << 8 = {})\n", unknown2, unknown2, unknown2 << 8);
	fmt::print("unknown3 = 0x{:02x} ({} << 8 = {})\n", unknown3, unknown3, unknown3 << 8);

	fmt::print("stream position = {}\n", s.tellg());

	std::vector<uint8_t> decompressed_data(data.size() * 20); // TODO: ???
	uLongf usize = decompressed_data.size();
	int r = uncompress((Bytef *)decompressed_data.data(), (uLongf *)&usize, (Bytef *)data.data() + s.tellg(), data.size() - s.tellg());
	if (r != Z_OK) {
		std::string o;
		switch (r) {
			case Z_MEM_ERROR: o = "Out of memory"; break;
			case Z_BUF_ERROR: o = "Out of buffer space"; break;
			case Z_DATA_ERROR: o = "Corrupt data"; break;
			default: o = "Unknown error"; break;
		}
		o = o + " while decompressing";
		fmt::print(stderr, "{}\n", o);
		exit(1);
	}

	fmt::print("decompressed data size = {}\n", usize);

	fs::path output_filename("creaturesarchive.out");
	fmt::print("writing to {}\n", output_filename.string());
	std::ofstream out(output_filename, std::ios::binary);
	out.write((char*)decompressed_data.data(), usize);
}