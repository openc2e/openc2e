#pragma once

#include <string>

inline std::string ascii_tolower(std::string s) {
	for (auto& c : s) {
		if (c <= 'Z' && c >= 'A')
			c = c - ('Z' - 'z');
	}
	return s;
}

inline std::wstring ascii_tolower(std::wstring s) {
	for (auto& c : s) {
		if (c <= 'Z' && c >= 'A')
			c = c - ('Z' - 'z');
	}
	return s;
}