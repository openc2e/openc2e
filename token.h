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

#include "openc2e.h"
#include "caosVar.h"
#include <vector>
#include <iostream>
#include <sstream>

void yyrestart(std::istream *stream, bool use_c2);

enum toktype { ANYTOKEN = 0, EOI = 0, TOK_CONST, TOK_WORD, TOK_BYTESTR };

struct token {
	int yyline;

	toktype type;
	bytestring_t bytestr;
	std::string word;
	caosVar constval;

	token() {}
	token(const token &cp) {
		yyline = cp.yyline;
		type = cp.type;
		bytestr = cp.bytestr;
		word = cp.word;
		constval = cp.constval;
	}

	std::string dump() {
		std::ostringstream oss;
		switch(type) {
			case TOK_CONST:
				oss << "constval ";
				if (constval.hasInt())
					oss << "int " << constval.getInt();
				else if (constval.hasFloat())
					oss << "float " << constval.getFloat();
				else if (constval.hasString())
					oss << "string " << constval.getString();
				else if (constval.hasAgent())
					oss << "agent (BAD!)";
				else
					oss << "(BAD)";
				break;
			case TOK_WORD:
				oss << "word " << word;
				break;
			case TOK_BYTESTR:
				{
					bytestring_t::iterator i = bytestr.begin();
					oss << "bytestr ";
					while (i != bytestr.end())
						oss << (int)*i++ << " ";
				}
				break;
			default:
				oss << "BROKEN";
				break;
		}
		oss << " (line " << yyline << ")";
		return oss.str();
	}
};

token *getToken(toktype expected = ANYTOKEN);
token *tokenPeek();

extern token lasttok; // internal use only

#endif

/* vim: set noet: */
