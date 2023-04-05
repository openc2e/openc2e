#include "fileformats/NewSFCFile.h"

#include <fmt/core.h>
#include <fstream>


int main(int argc, char** argv) {
	if (argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		fmt::print(stderr, "USAGE: {} FILE\n", argv[0]);
		return 1;
	}

	auto input_filename = argv[1];

	std::ifstream in(input_filename, std::ios_base::binary);
	auto sfc = sfc::read_sfc_v1_file(in);

	// check if we read all of it
	in.peek();
	fmt::print("read entire file? {}\n", in.eof());

	fmt::print("number of objects {}\n", sfc.objects.size());
	fmt::print("number of sceneries {}\n", sfc.sceneries.size());
	fmt::print("number of scripts {}\n", sfc.scripts.size());
	fmt::print("number of macros {}\n", sfc.macros.size());
}
