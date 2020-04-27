#pragma once

#include <string>

std::string cp1252_to_utf8(const std::string& cp1252_str);
std::string utf8_to_cp1252(const std::string& utf8_str);

// If a string is UTF-8, simply return a copy (minus any BOM), otherwise assume
// it's CP-1252 and convert it.
std::string ensure_utf8(const std::string& str);

// If a string is UTF-8, convert it, otherwise assume it's already in CP-1252.
std::string ensure_cp1252(const std::string& str);
