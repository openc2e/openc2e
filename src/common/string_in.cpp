#include "string_in.h"

bool string_in(const std::string& value, std::initializer_list<std::string> strings) {
	for (auto s : strings) {
		if (value == s) {
			return true;
		}
	}
	return false;
}