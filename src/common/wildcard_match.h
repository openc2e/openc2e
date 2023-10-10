#pragma once

#include "StringView.h"

bool wildcard_match(StringView pattern, StringView value) {
	while (true) {
		if (pattern.empty() && value.empty()) {
			return true;
		}
		if (pattern.empty()) {
			return false;
		}
		if (value.empty()) {
			return pattern.size() == 1 && pattern[0] == '*';
		}
		if (pattern[0] == '?' || pattern[0] == value[0]) {
			pattern = pattern.substr(1);
			value = value.substr(1);
			continue;
		}
		if (pattern[0] == '*') {
			// TODO: this recursive/branching implementation is ugly. can we do it linearly
			// without using too much extra memory? maybe using a static vector?
			return wildcard_match(pattern, value.substr(1)) || wildcard_match(pattern.substr(1), value);
		}
		return false;
	}
}