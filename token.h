#ifndef TOKEN_H
#define TOKEN_H 1

#include "caosVar.h"
#include <vector>
#include <iostream>
#include <sstream>

void yyrestart(std::istream *stream);

enum toktype { ANYTOKEN = 0, EOI = 0, TOK_CONST, TOK_WORD, TOK_BYTESTR };

struct token {
	int yyline;

	toktype type;
	std::vector<unsigned int> bytestr;
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
					std::vector<unsigned int>::iterator i = bytestr.begin();
					oss << "bytestr ";
					while (i != bytestr.end())
						oss << *i++ << " ";
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
