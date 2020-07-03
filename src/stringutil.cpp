#include "stringutil.h"
#include <algorithm>
#include <ctype.h>

bool string_in(const std::string& value, std::initializer_list<std::string> strings) {
    for (auto s : strings) {
        if (value == s) {
            return true;
        }
    }
    return false;
}

std::string lowerstring(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), tolower);
    return s;
}

std::string upperstring(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), toupper);
    return s;
}