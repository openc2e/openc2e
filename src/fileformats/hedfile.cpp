#include "hedfile.h"

#include "common/endianlove.h"
#include "common/throw_ifnot.h"

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

	THROW_IFNOT(!in.fail());
	in.peek();
	THROW_IFNOT(in.eof());

	return hed;
}