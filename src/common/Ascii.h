#pragma once

#include <string>

inline bool is_ascii_digit(char c) {
	return c >= '0' && c <= '9';
}

inline char to_ascii_lowercase(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - ('Z' - 'z');
	} else {
		return c;
	}
}

inline wchar_t to_ascii_lowercase(wchar_t c) {
	if (c >= 'A' && c <= 'Z') {
		return c - ('Z' - 'z');
	} else {
		return c;
	}
}

inline std::string to_ascii_lowercase(std::string s) {
	for (auto& c : s) {
		c = to_ascii_lowercase(c);
	}
	return s;
}

inline std::wstring to_ascii_lowercase(std::wstring s) {
	for (auto& c : s) {
		c = to_ascii_lowercase(c);
	}
	return s;
}