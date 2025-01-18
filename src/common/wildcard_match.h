#pragma once

#include "Ascii.h"
#include "StringView.h"

bool wildcard_match_ignore_case(StringView pattern, StringView value) {
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
		if (pattern[0] == '?' || to_ascii_lowercase(pattern[0]) == to_ascii_lowercase(value[0])) {
			pattern = pattern.substr(1);
			value = value.substr(1);
			continue;
		}
		if (pattern[0] == '*') {
			// TODO: this recursive/branching implementation is ugly. can we do it linearly
			// without using too much extra memory? maybe using a static vector?
			return wildcard_match_ignore_case(pattern, value.substr(1)) || wildcard_match_ignore_case(pattern.substr(1), value);
		}
		return false;
	}
}