#include "Repr.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>

std::string repr(const std::string& s) {
	std::string result;
	result += '\'';
	for (uint8_t c : s) {
		if (c == '\r') {
			result += "\\r";
		} else if (c == '\n') {
			result += "\\n";
		} else if (c == '\t') {
			result += "\\t";
		} else if (c == '\'') {
			result += "\\'";
		} else if (c == '\\') {
			result += "\\\\";
		} else if (c < 0x20 || c >= 0x7f) {
			// TODO: what about printable unicode?
			result += fmt::format("\\x{:02x}", c);
		} else {
			result += (char)c;
		}
	}
	result += '\'';
	return result;
}

std::string repr(const ghc::filesystem::path& p) {
	return repr(p.string());
}