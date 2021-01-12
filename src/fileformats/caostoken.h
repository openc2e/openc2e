/*
 *  caostoken.h
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

#pragma once

#include <string>
#include <vector>

struct caostoken {
  public:
	enum toktype {
		TOK_WORD,
		TOK_BYTESTR,
		TOK_STRING,
		TOK_CHAR,
		TOK_BINARY,
		TOK_INT,
		TOK_FLOAT,
		TOK_COMMENT,
		TOK_WHITESPACE,
		TOK_NEWLINE,
		TOK_COMMA,
		TOK_EOI,
		TOK_ERROR
	};

	toktype type;
	std::string value;

	int index;
	int lineno;

	caostoken()
		: lineno(-1) {}
	caostoken(toktype type_, int lineno_)
		: type(type_), lineno(lineno_) {}
	caostoken(toktype type_, std::string value_, int lineno_)
		: type(type_), value(value_), lineno(lineno_) {}

	const std::string& word() const {
		if (type != TOK_WORD)
			abort();
		return value;
	}

	void setWord(const std::string& s) {
		type = TOK_WORD;
		value = s;
	}

	std::string stringval() const;
	std::vector<unsigned char> bytestr() const;
	int intval() const;
	float floatval() const;

	std::string format() const;
	std::string typeAsString() const;
};
