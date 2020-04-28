#pragma once

#include "encoding.h"

char32_t cp1252_to_codepoint(unsigned char cp1252_char);
unsigned char unicode_to_cp1252(char32_t codepoint);