#include "strFile.h"

#include "common/Exception.h"
#include "common/endianlove.h"
#include "common/io/FileReader.h"

std::vector<std::string> ReadStrFile(const std::string& path) {
	FileReader in(path);

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