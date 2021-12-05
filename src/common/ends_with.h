#pragma once

#include "common/ascii_tolower.h"

#include <string>

inline bool ends_with_ignore_case(const std::string& s, const std::string& suffix) {
	if (suffix.size() > s.size()) {
		return false;
	}
	for (size_t i = 0; i < suffix.size(); ++i) {
		if (ascii_tolower(s[s.size() - suffix.size() + i]) != ascii_tolower(suffix[i])) {
			return false;
		}
	}
	return true;
}
