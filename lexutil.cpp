/*
 *  lexutil.cpp
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
#include "lexutil.h"
#include "lex.yy.h"
#include "lex.c2.h"
#include "token.h"
#include <vector>
#include <string>

int lex_lineno;

bytestring_t bytestr;
std::string temp_str;

static token *peektok = NULL;
token lasttok;

static c2eFlexLexer *lexer = NULL; // XXX!
static c2FlexLexer *c2lexer = NULL;

bool using_c2;

void yyrestart(std::istream *is, bool use_c2) {
	using_c2 = use_c2;

	if (lexer) delete lexer;
	if (c2lexer) delete c2lexer;
	
	lexreset();
	
	if (using_c2) {
		c2lexer = new c2FlexLexer();
		c2lexer->yyrestart(is);
	} else {
		lexer = new c2eFlexLexer();
		lexer->yyrestart(is);
	}
}

void lexreset() {
	bytestr.clear();
	temp_str = "";
	peektok = NULL;
}

token *getToken(toktype expected) {
	token *ret = tokenPeek();
	peektok = NULL;
	if (expected != ANYTOKEN && ret->type != expected)
		throw parseException("unexpected token");
	return ret;
}

token *tokenPeek() {
	if (peektok)
		return peektok;
	if (using_c2) {
		assert(c2lexer);
		if (!c2lexer->yylex())
			return NULL;
	} else {
		assert(lexer);
		if (!lexer->yylex())
			return NULL;
	}
	peektok = &lasttok;
	return peektok;
}
/* vim: set noet: */
