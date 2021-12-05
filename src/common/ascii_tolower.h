#pragma once

#include <string>

inline char ascii_tolower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - ('Z' - 'z');
	} else {
		return c;
	}
}

inline wchar_t ascii_tolower(wchar_t c) {
	if (c >= 'A' && c <= 'Z') {
		return c - ('Z' - 'z');
	} else {
		return c;
	}
}

inline std::string ascii_tolower(std::string s) {
	for (auto& c : s) {
		c = ascii_tolower(c);
	}
	return s;
}

inline std::wstring ascii_tolower(std::wstring s) {
	for (auto& c : s) {
		c = ascii_tolower(c);
	}
	return s;
}