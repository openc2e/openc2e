#include "verticalbarFile.h"

#include "common/io/FileReader.h"
#include "common/io/Getline.h"

std::vector<std::vector<std::string>> ReadVerticalBarSeparatedValuesFile(const std::string& path) {
	FileReader in(path);

	std::vector<std::vector<std::string>> lines;
	while (true) {
		std::string line = getline(in);
		if (line.empty() && !in.has_data_left()) {
			break;
		}
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
