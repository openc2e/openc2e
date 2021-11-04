#include "strFile.h"

#include "common/Exception.h"
#include "common/endianlove.h"

#include <fstream>

std::vector<std::string> ReadStrFile(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	if (!in.good()) {
		throw Exception("Couldn't open " + path);
	}

	int num_strings = read16le(in);

	std::vector<std::string> strings(num_strings);
	for (int i = 0; i < num_strings; i++) {
		int len = read8(in);
		if (len == 0)
			continue;

		strings[i].resize(len);
		in.read(&strings[i][0], len);
	}
	return strings;
}