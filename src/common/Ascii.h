#pragma once

#include <string>

inline bool is_ascii_digit(char c) {
	return c >= '0' && c <= '9';
}

inline bool is_ascii_alpha(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline bool is_ascii_alnum(char c) {
	return is_ascii_digit(c) || is_ascii_alpha(c);
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