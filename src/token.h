/*
 *  token.h
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
#ifndef TOKEN_H
#define TOKEN_H 1

#include <vector>
#include <sstream>

struct token {
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
		TOK_EOI,
		TOK_ERROR
	};

	toktype type;
	std::string strval;
	int intval;
	float floatval;
	std::vector<unsigned char> bytestrval;

	int index;
	int lineno;

	void setWord(const std::string& s) {
		type = TOK_WORD;
		strval = s;
	}

	const std::vector<unsigned char> &bytestr() const {
		if (type != TOK_BYTESTR) abort();
		return bytestrval;
	}

	const std::string &word() const {
		if (type != TOK_WORD) abort();
		return strval;
	}

	token() : lineno(-1) {}
	token(toktype type_, int lineno_) :
		type(type_), lineno(lineno_) {}
	token(toktype type_, std::string strval_, int lineno_) :
		type(type_), strval(strval_), lineno(lineno_) {}
	token(toktype type_, int intval_, int lineno_) :
		type(type_), intval(intval_), lineno(lineno_) {}
	token(toktype type_, float floatval_, int lineno_) :
		type(type_), floatval(floatval_), lineno(lineno_) {}
	token(toktype type_, std::vector<unsigned char> bytestrval_, int lineno_) :
		type(type_), bytestrval(bytestrval_), lineno(lineno_) {}

	std::string format() const {
		switch(type) {
		case TOK_EOI: return "<EOI>";
		case TOK_ERROR: return "<ERROR>";
		case TOK_INT:
		case TOK_BINARY:
		case TOK_CHAR:
			return std::to_string(intval);
		case TOK_FLOAT:
			return std::to_string(floatval);
		case TOK_STRING:
		{
			std::ostringstream outbuf;
			std::string inbuf = strval;
			outbuf << '"';
			for (size_t i = 0; i < inbuf.size(); i++) {
				switch (inbuf[i]) {
					case '\n': outbuf << "\\\n"; break;
					case '\r': outbuf << "\\\r"; break;
					case '\t': outbuf << "\\\t"; break;
					case '\"': outbuf << "\\\""; break;
					default:   outbuf << inbuf[i]; break;
				}
			}
			outbuf << '"';
			return outbuf.str();
		}
		case TOK_WORD:
			return strval;
		case TOK_COMMENT:
			return strval;
		case TOK_BYTESTR:
		{
			std::ostringstream oss;
			oss << "[ ";
			for (size_t i = 0; i < bytestrval.size(); ++i)
				oss << std::to_string(bytestrval[i]) << " ";
			oss << "]";
			return oss.str();
		}
		}
	}

	std::string typeAsString() const {
		switch(type) {
		case TOK_WORD: return "word";
		case TOK_BYTESTR: return "bytestr";
		case TOK_STRING: return "string";
		case TOK_CHAR: return "char";
		case TOK_BINARY: return "binary";
		case TOK_INT: return "integer";
		case TOK_FLOAT: return "float";
		case TOK_COMMENT: return "comment";
		case TOK_EOI: return "eoi";
		case TOK_ERROR: return "lexer error";
		default: abort();
		}
	}
};

#endif

/* vim: set noet: */
