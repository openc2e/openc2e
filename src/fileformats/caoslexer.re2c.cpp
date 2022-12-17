#include "caoslexer.h"
#include "caostoken.h"
#include "common/SimpleLexer.h"

#include <algorithm>

void lexcaos(std::vector<caostoken>& v, const std::string& p) {
	return lexcaos(v, p.c_str());
}

void lexcaos(std::vector<caostoken>& v, const char* script) {
	SimpleLexerConfiguration conf;
	conf.asterisk_comments = true;
	// conf.symbols = ",<=>";
	auto tokens = simplelex(script, conf);

	SimpleToken* p = tokens.data();
	int yylineno = 1;

	while (true) {
		if (p[0].type == SIMPLE_NEWLINE) {
			v.push_back(caostoken(caostoken::TOK_NEWLINE, p[0].raw_value, yylineno));
			yylineno++;
			p++;
		} else if (p[0].type == SIMPLE_WHITESPACE) {
			v.push_back(caostoken(caostoken::TOK_WHITESPACE, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_COMMENT) {
			v.push_back(caostoken(caostoken::TOK_COMMENT, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].is_symbol(',')) {
			v.push_back(caostoken(caostoken::TOK_COMMA, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_CHAR) {
			v.push_back(caostoken(caostoken::TOK_CHAR, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_BAREWORD) {
			v.push_back(caostoken(caostoken::TOK_WORD, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_BINARY) {
			v.push_back(caostoken(caostoken::TOK_BINARY, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_NUMBER) {
			if (p[0].is_integer()) {
				v.push_back(caostoken(caostoken::TOK_INT, p[0].raw_value, yylineno));
			} else {
				v.push_back(caostoken(caostoken::TOK_FLOAT, p[0].raw_value, yylineno));
			}
			p++;
		} else if (p[0].is_symbol('<') && p[1].is_symbol('>')) {
			v.push_back(caostoken(caostoken::TOK_WORD, "<>", yylineno));
			p += 2;
		} else if (p[0].is_symbol('<') && p[1].is_symbol('=')) {
			v.push_back(caostoken(caostoken::TOK_WORD, "<=", yylineno));
			p += 2;
		} else if (p[0].is_symbol('>') && p[1].is_symbol('=')) {
			v.push_back(caostoken(caostoken::TOK_WORD, ">=", yylineno));
			p += 2;
		} else if (p[0].is_symbol('<')) {
			v.push_back(caostoken(caostoken::TOK_WORD, "<", yylineno));
			p++;
		} else if (p[0].is_symbol('>')) {
			v.push_back(caostoken(caostoken::TOK_WORD, ">", yylineno));
			p++;
		} else if (p[0].is_symbol('=')) {
			v.push_back(caostoken(caostoken::TOK_WORD, "=", yylineno));
			p++;
		} else if (p[0].type == SIMPLE_BYTESTR) {
			v.push_back(caostoken(caostoken::TOK_BYTESTR, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_STRING) {
			v.push_back(caostoken(caostoken::TOK_STRING, p[0].raw_value, yylineno));
			p++;
		} else if (p[0].type == SIMPLE_EOI) {
			v.push_back(caostoken(caostoken::TOK_EOI, p[0].raw_value, yylineno));
			break;
		} else {
			v.push_back(caostoken(caostoken::TOK_ERROR, p[0].raw_value, yylineno));
			break;
		}
	}

	for (size_t i = 0; i < v.size(); i++)
		v[i].index = i;
}
