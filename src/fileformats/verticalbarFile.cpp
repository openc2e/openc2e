#include "verticalbarFile.h"

#include "common/Exception.h"

#include <fstream>

std::vector<std::vector<std::string>> ReadVerticalBarSeparatedValuesFile(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	if (!in.good()) {
		throw Exception("Couldn't open " + path);
	}

	std::vector<std::vector<std::string>> lines;
	while (in.good()) {
		std::string line;
		getline(in, line);
		if (line.empty() || line[0] == '#') {
			continue;
		}

		std::vector<std::string> values;
		auto start = 0;
		auto end = line.find('|');
		while (true) {
			values.push_back(line.substr(start, end - start));
			if (end == std::string::npos)
				break;
			start = end + 1;
			end = line.find('|', start);
		}
		lines.push_back(values);
	}
	return lines;
}