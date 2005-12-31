#include "lexutil.h"
#include "lex.yy.h"
#include "token.h"
#include <vector>
#include <string>

int lex_lineno;

std::vector<unsigned int> bytestr;
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
