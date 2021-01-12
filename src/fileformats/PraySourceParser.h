#pragma once

#include <mpark/variant.hpp>
#include <string>
#include <vector>

namespace PraySourceParser {
struct Error {
	std::string message;
};
struct GroupBlockStart {
	std::string type;
	std::string name;
};
struct GroupBlockEnd {
	std::string type;
	std::string name;
};
struct InlineBlock {
	std::string type;
	std::string name;
	std::string filename;
};
struct StringTag {
	std::string key;
	std::string value;
};
struct StringTagFromFile {
	std::string key;
	std::string filename;
};
struct IntegerTag {
	std::string key;
	int value;
};
using Event =
	mpark::variant<Error, GroupBlockStart, GroupBlockEnd,
		InlineBlock, StringTag, StringTagFromFile, IntegerTag>;

std::vector<Event> parse(const std::string&);
std::string eventToString(const Event&);
} // namespace PraySourceParser
