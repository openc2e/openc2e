#include "Repr.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>

std::string repr(char c) {
	if (c == '\r') {
		return "\\r";
	} else if (c == '\n') {
		return "\\n";
	} else if (c == '\t') {
		return "\\t";
	} else if (c == '\'') {
		return "\\'";
	} else if (c == '\\') {
		return "\\\\";
	} else if (static_cast<uint8_t>(c) < 0x20 || static_cast<uint8_t>(c) >= 0x7f) {
		// TODO: what about printable unicode?
		return fmt::format("\\x{:02x}", static_cast<uint8_t>(c));
	} else {
		std::string s;
		s += c;
		return s;
	}
}

std::string repr(const std::string& s) {
	std::string result;
	result += '\'';
	for (char c : s) {
		result += repr(c);
	}
	result += '\'';
	return result;
}

std::string repr(const ghc::filesystem::path& p) {
	return repr(p.string());
}