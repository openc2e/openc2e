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
#include "token.h"
#include <vector>
#include <string>

int lex_lineno;

bytestring_t bytestr;
std::string temp_str;

static token *peektok = NULL;
token lasttok;

static yyFlexLexer *lexer = NULL; // XXX!

void yyrestart(std::istream *is) {
	if (lexer)
		delete lexer;
	lexer = new yyFlexLexer();
	lexreset();
	lexer->yyrestart(is);
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
	if (!lexer->yylex())
		return NULL;
	peektok = &lasttok;
	return peektok;
}
/* vim: set noet: */
