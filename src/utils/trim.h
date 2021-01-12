#pragma once

#include <string>

inline std::string trim(std::string s) {
	size_t startpos = s.find_first_not_of(" ");
	if (startpos == std::string::npos) {
		return {};
	}
	size_t endpos = s.find_last_not_of(" ");
	return s.substr(startpos, endpos + 1);
}