#include "dialect.h"
#include "lex.yy.h"
#include "token.h"
#include <string>
#include <cctype>
#include <cmath>
#include "caosScript.h"
#include "cmddata.h"
#include "exceptions.h"
#include "caosVar.h"
#include "Agent.h"
#include "bytecode.h"

#include <sstream>
#include <boost/format.hpp>

using boost::str;

std::map<std::string, Variant *> variants;

/*
 * If the inverse of an AND condition succeeds, jump to next OR (foo) bit, or
 * fail.
 * 
 * If an OR condition succeeds, jump to next AND (foo) or succeed.
 *
 * At the end, if the last condition was AND, succeed. Else, fail.
 *
 * The first condition is considered to be an AND.
 *
 * gogo relocations
 */
void parseCondition(caosScript *s, int success, int failure) {
	bool wasAnd = true;
	int nextAnd, nextOr;
	nextAnd = s->current->newRelocation();
	nextOr  = s->current->newRelocation();
	while(1) {
		int entry = s->current->getNextIndex();
		s->v->exp_dialect->doParse(s);
		
		token *comparison = getToken(TOK_WORD);
		std::string cword = comparison->word;
		int compar;
		if (cword == "eq")
			compar = CEQ;
		else if (cword == "gt")
			compar = CGT;
		else if (cword == "ge")
			compar = CGE;
		else if (cword == "lt")
			compar = CLT;
		else if (cword == "le")
			compar = CLE;
		else if (cword == "ne")
			compar = CNE;
		s->v->exp_dialect->doParse(s);

		bool isOr = true;
		bool isLast = false;

		struct token *peek = tokenPeek();
		if (!peek)
			isLast = true;
		else if (peek->type == TOK_WORD) {
			if (peek->word == "and") {
				getToken();
				isOr = false;
			} else if (peek->word == "or")
				getToken();
			else isLast = true;
		}

		if (!wasAnd) {
			s->current->fixRelocation(nextOr, entry);
			nextOr = s->current->newRelocation();
		} else {
			s->current->fixRelocation(nextAnd, entry);
			nextAnd = s->current->newRelocation();
		}
		
		int jumpTarget = isOr ? nextAnd : nextOr;
		if (!isOr) compar = ~compar & CMASK;
		
		s->current->thread(new caosCond(compar, jumpTarget));
		wasAnd = !isOr;
		
		if (isLast) break;
	}
	s->current->fixRelocation(nextAnd, success);
	s->current->fixRelocation(nextOr,  failure);
	s->current->thread(new caosJMP(wasAnd ? success : failure));
}

void DefaultParser::operator()(class caosScript *s, class Dialect *curD) {
	int argc = cd->argc;
	for (int i = 0; i < argc; i++) {
		if (cd->argtypes)
			s->v->exp_dialect->set_expect(cd->argtypes[i]);
		s->v->exp_dialect->doParse(s);
	}
	s->current->thread(new simpleCaosOp(cd));
}


bool Dialect::parseOne(caosScript *s) {
	token *t;
	t = getToken();
	if (!t) {
		eof();
		return false;
	}
	handleToken(s, t);
	return true;
}

void Dialect::handleToken(caosScript *s, token *t) {
	if (t->type != TOK_WORD)
		throw parseFailure(std::string("unexpected non-word ") + t->dump());
	std::string word = t->word;
	if (delegates.find(word) == delegates.end())
		throw parseException(std::string("no delegate for ") + t->dump());
	parseDelegate &p = *delegates[word];
	p(s, this, expect);
}

void ExprDialect::handleToken(caosScript *s, token *t) {
	switch (t->type) {
		case TOK_CONST:
			s->current->thread(new ConstOp(t->constval));
			break;
		case TOK_WORD:
			{
				std::string word = t->word;
				if (word.size() == 4) {
					if (word[0] == 'v' && word[1] == 'a') {
						if(!(isdigit(word[2]) && isdigit(word[3])))
							throw parseException("bad vaxx");
						s->current->thread(new opVAxx(atoi(word.c_str() + 2)));
						return;
					} else if (word[0] == 'o' && word[1] == 'v') {
						if(!(isdigit(word[2]) && isdigit(word[3])))
							throw parseException("bad ovxx");
						s->current->thread(new opOVxx(atoi(word.c_str() + 2)));
						return;
					} else if (word[0] == 'm' && word[1] == 'v') {
						if(!(isdigit(word[2]) && isdigit(word[3])))
							throw parseException("bad mvxx");
						s->current->thread(new opMVxx(atoi(word.c_str() + 2)));
						return;
					}
				}
			}
			Dialect::handleToken(s, t);
			return;
		case TOK_BYTESTR:
			s->current->thread(new opBytestr(t->bytestr));
			break;
		default:
			assert(false);
	}
}

void DoifDialect::handleToken(class caosScript *s, token *t) {
	if (t->type == TOK_WORD) {
		if (t->word == "endi") {
			if (failure) // we don't have an else clause
				s->current->fixRelocation(failure);
			stop = true;
			return;
		}
		if (t->word == "else") {
			if (!failure)
				throw new parseException("double else clause is forbidden");
			s->current->thread(new caosJMP(exit));
			s->current->fixRelocation(failure);
			failure = 0;
			return;
		}
		if (t->word == "elif") {
			// emuluate an else-doif-endi block
			if (!failure)
				throw new parseException("double else clause is forbidden");
			s->current->thread(new caosJMP(exit));
			s->current->fixRelocation(failure);
			failure = 0;
			DoifParser dip;
			dip(s, this);
			stop = true;
			return;
		}
	}
	Dialect::handleToken(s, t);
}

std::map<std::string, const cmdinfo *> op_key_map;

void registerDelegates() {
	registerAutoDelegates();

	std::map<std::string, Variant *>::iterator it = variants.begin();
	while (it != variants.end()) {
		Variant *v = (*it).second;
		v->name = (*it).first;
		const cmdinfo *cmd = v->cmds;

		while (cmd->key) {
			op_key_map[std::string(cmd->key)] = cmd;
			cmd++;
		}

		it++;
	}
}
/* vim: set noet: */
