#pragma once

#include <string>

inline bool wildcard_match_helper(const char* pattern, const char* value) {
	// TODO: string_view would be nicer than raw char pointers, and allow us to
	// use only a single function...
	while (true) {
		if (*pattern == '\0' && *value == '\0') {
			return true;
		}
		if (*pattern == '*' && *(pattern + 1) != '\0' && *value == '\0') {
			return false;
		}
		if (*pattern == '?' || *pattern == *value) {
			pattern += 1;
			value += 1;
			continue;
		}
		if (*pattern == '*') {
			return wildcard_match_helper(pattern, value + 1) || wildcard_match_helper(pattern + 1, value);
		}
		return false;
	}
}

inline bool wildcard_match(const std::string& pattern, const std::string& value) {
	return wildcard_match_helper(pattern.c_str(), value.c_str());
}