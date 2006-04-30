/*
 *  lexutil.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */
#ifndef LEXUTIL_H
#define LEXUTIL_H 1

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <string>
#include <cstdio>
#include <cctype>
#include <vector>
#include <algorithm>

#include "token.h"

extern int lex_lineno;

void lexreset();

extern bytestring_t bytestr;
extern std::string temp_str;

static inline int make_int(int v) {
	lasttok.type = TOK_CONST;
	lasttok.constval.setInt(v);
	lasttok.yyline = lex_lineno;
	return 1;
}

static inline int make_bin(const char *str) {
	int temp = 0, pos;
	str++; // skip %
	for (pos = 0; pos < 8; pos++) {
		temp <<= 1;
		temp += str[pos] - '0';
	}
	return make_int(temp);
//    return temp;
}

static inline int make_float(float f) {
	lasttok.type = TOK_CONST;
	lasttok.constval.setFloat(f);
	lasttok.yyline = lex_lineno;
	return 1;
}

static inline int make_word(const char *str) {
	std::string result = str;
	lasttok.type = TOK_WORD;
	std::transform(result.begin(), result.end(), result.begin(), (int(*)(int))tolower);
	lasttok.word = result;
	lasttok.yyline = lex_lineno;
	return 1;
}

static inline void push_string_escape(char c) {
	switch (c) {
		case 'n':
			temp_str += '\n';
			break;
		case 'r':
			temp_str += '\r';
			break;
		case 't':
			temp_str += '\t';
			break;
		default:
			temp_str += c;
			break;
	}
}

static inline void push_string_lit(char c) {
	temp_str += c;
}

static inline int make_string() {
	lasttok.type = TOK_CONST;
	lasttok.constval.setString(temp_str);
	temp_str = "";
	lasttok.yyline = lex_lineno;
	return 1;
}

static inline int push_bytestr(unsigned int bs) {
	if (bs > 255) {
		std::ostringstream oss;
		oss << "Byte string element out of range (0 <= " << bs << " < 256) at line " << lex_lineno;
		throw new parseException(oss.str());
	}
	bytestr.push_back(bs);
	lasttok.yyline = lex_lineno;
	return 1;
}

static inline int make_bytestr() {
	lasttok.type = TOK_BYTESTR;
	lasttok.bytestr = bytestr;
	bytestr.clear();
	lasttok.yyline = lex_lineno;
	return 1;
}

static inline void parse_error(const char *yytext, int yylineno) {
	std::ostringstream oss;
	oss << "Parse error at line " << yylineno << ", near " << yytext;
	throw parseException(oss.str());
}

#endif

/* vim: set noet: */
