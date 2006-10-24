/*
 *  dialect.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
 * Since DOIFs don't short-circuit, just keep a flag indicating whether
 * the current condition is true or not on the stack...
 */
void parseCondition(caosScript *s, int success, int failure) {
	bool isAnd = true;
	s->current->thread(new ConstOp(caosVar(1)));
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

		bool nextIsAnd = false;
		bool isLast = false;

		struct token *peek = tokenPeek();
		if (!peek)
			isLast = true;
		else if (peek->type == TOK_WORD) {
			if (peek->word == "and") {
				getToken();
				nextIsAnd = true;
			} else if (peek->word == "or")
				getToken();
			else isLast = true;
		}
		
		s->current->thread(new caosCond(compar, isAnd));
		isAnd = nextIsAnd;
		
		if (isLast) break;
	}
	s->current->thread(new caosCJMP(success));
	s->current->thread(new caosJMP(failure));
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
				if (expect == CI_BAREWORD) {
					s->current->thread(new ConstOp(caosVar(word)));
					return;
				}
				if (word.size() == 4) {
					if (word[0] == 'v' && word[1] == 'a') {
						if (word[2] == 'r') {
							if (!isdigit(word[3]))
								throw parseException("non-digits found in VARx");
							s->current->thread(new opVAxx(atoi(word.c_str() + 3)));
						} else {
							if (!(isdigit(word[2]) && isdigit(word[3])))
								throw parseException("non-digits found in VAxx");
							s->current->thread(new opVAxx(atoi(word.c_str() + 2)));
						}
						return;
					} else if (word[0] == 'o' && word[1] == 'v') {
						if(!(isdigit(word[2]) && isdigit(word[3])))
							throw parseException("non-digits found in OVxx");
						s->current->thread(new opOVxx(atoi(word.c_str() + 2)));
						return;
					} else if (word[0] == 'o' && word[1] == 'b' && word[2] == 'v') {
						if (!isdigit(word[3]))
							throw parseException("non-digits found in OBVx");
						s->current->thread(new opOVxx(atoi(word.c_str() + 3)));
						return;
					} else if (word[0] == 'm' && word[1] == 'v') {
						if(!(isdigit(word[2]) && isdigit(word[3])))
							throw parseException("non-digits found in MVxx");
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
				throw parseException("double else clause is forbidden");
			s->current->thread(new caosJMP(exit));
			s->current->fixRelocation(failure);
			failure = 0;
			return;
		}
		if (t->word == "elif") {
			// emuluate an else-doif-endi block
			if (!failure)
				throw parseException("double else clause is forbidden");
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

class HackySETVforC2 : public parseDelegate {
	protected:
		NamespaceDelegate *nd;
		parseDelegate *pd;
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			token *t = tokenPeek();
			if (t->type == TOK_WORD && nd->dialect.delegates.find(t->word) != nd->dialect.delegates.end()) {
				(*nd)(s, curD);
			} else {
				(*pd)(s, curD);
			}
		}

		HackySETVforC2(NamespaceDelegate *n, parseDelegate *p) : nd(n), pd(p) {}
};

void registerDelegates() {
	registerAutoDelegates();

	variants[std::string("c2")]->cmd_dialect->delegates["setv"] = 
		new HackySETVforC2(
				(NamespaceDelegate *)(variants[std::string("c2")]->cmd_dialect->delegates["setv"]),
				variants[std::string("c3")]->cmd_dialect->delegates["setv"]
				);

	std::map<std::string, Variant *>::iterator it = variants.begin();
	while (it != variants.end()) {
		Variant *v = (*it).second;
		v->name = (*it).first;
		const cmdinfo *cmd = v->cmds;

		for (int i = 0; cmd[i].key; i++) {
			op_key_map[std::string(cmd[i].key)] = &cmd[i];
		}

		it++;
	}
}
/* vim: set noet: */
