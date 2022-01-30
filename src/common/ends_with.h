#pragma once

#include "common/Ascii.h"

#include <string>

inline bool ends_with_ignore_case(const std::string& s, const std::string& suffix) {
	if (suffix.size() > s.size()) {
		return false;
	}
	for (size_t i = 0; i < suffix.size(); ++i) {
		if (to_ascii_lowercase(s[s.size() - suffix.size() + i]) != to_ascii_lowercase(suffix[i])) {
			return false;
		}
	}
	return true;
}
