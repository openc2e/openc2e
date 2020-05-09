#pragma once

#include <string>

// If a string is UTF-8, simply return a copy (minus any BOM), otherwise assume
// it's CP-1252 and convert it.
std::string ensure_utf8(const std::string& str);

// If a string is UTF-8, convert it, otherwise assume it's already in CP-1252.
std::string ensure_cp1252(const std::string& str);

// translate strings, throwing if it's not possible
std::string cp1252_to_utf8(const std::string& cp1252_str);
std::string utf8_to_cp1252(const std::string& utf8_str);

// translate characters, throwing if it's not possible
char32_t cp1252_to_codepoint(unsigned char cp1252_char);
unsigned char unicode_to_cp1252(char32_t codepoint);

// helper functions
bool cp1252_isprint(unsigned char cp1252_char);