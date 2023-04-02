#pragma once

#include "common/Ascii.h"

#include <assert.h>
#include <initializer_list>
#include <string>
#include <vector>


enum SimpleTokenType {
	SIMPLE_BAREWORD,
	SIMPLE_STRING,
	SIMPLE_NUMBER,
	SIMPLE_WHITESPACE,
	SIMPLE_COMMENT,
	SIMPLE_BYTESTR,
	SIMPLE_BINARY,
	SIMPLE_CHAR,
	SIMPLE_NEWLINE,
	SIMPLE_ERROR,
	SIMPLE_EOI
};

struct SimpleToken {
	SimpleToken()
		: type(SIMPLE_ERROR), lineno(-1) {}
	SimpleToken(SimpleTokenType type_, int lineno_, std::string raw_value_)
		: type(type_), lineno(lineno_), raw_value(raw_value_) {}
	SimpleTokenType type;
	int lineno;
	std::string raw_value;

	std::string value() const;
	int integer_value() const;

	bool is_one_of(std::initializer_list<SimpleTokenType>) const;
	bool is_integer() const;
	bool is_symbol(char c) const;

	std::string repr() const;
	const char* c_str() const;
};

struct SimpleLexerConfiguration {
	bool asterisk_comments = false;
	bool paren_asterisk_comments = false;
};

std::vector<SimpleToken> simplelex(const char* p, SimpleLexerConfiguration = {});