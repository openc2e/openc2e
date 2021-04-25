#include "hedfile.h"

#include "caos_assert.h"
#include "utils/endianlove.h"

#include <fstream>

hedfile read_hedfile(const std::string& path) {
	std::ifstream in(path, std::ios::binary);
	return read_hedfile(in);
}

hedfile read_hedfile(std::istream& in) {
	hedfile hed;

	hed.frame_width = read32le(in);
	hed.frame_height = read32le(in);
	hed.numframes = read32le(in);

	caos_assert(!in.fail());
	in.peek();
	caos_assert(in.eof());

	return hed;
}