#pragma once

#include <string>

inline bool iendswith(const std::string& s, const std::string& pattern) {
	return std::equal(s.begin() + (s.size() - pattern.size()), s.end(), pattern.begin(), pattern.end(),
		[](char a, char b) { return std::tolower(a) == std::tolower(b); });
}