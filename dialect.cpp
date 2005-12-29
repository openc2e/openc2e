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
#include "caosVM.h"

std::map<std::string, Variant *> variants;

class DoifDialect : public Dialect {
	protected:
		int success, failure, exit;
	public:
		DoifDialect(caosScript *scr, int s, int f, int e)
			: success(s), failure(f), exit(e)
			{
				delegates = scr->v->cmd_dialect->delegates; // XXX
			}
		void handleToken(class caosScript *s, token *t); 
};

void DoifParser::operator()(class caosScript *s, class Dialect *curD) {
	int success, failure, exit;
	success = s->current->newRelocation();
	failure = s->current->newRelocation();
	exit    = s->current->newRelocation();
	
	parseCondition(s, success, failure);
	
	DoifDialect d(s, success, failure, exit);
	s->current->fixRelocation(success);
	d.doParse(s);
	s->current->fixRelocation(exit);
}

void AssertParser::operator()(class caosScript *s, class Dialect *curD) {
	int success, failure;
	success = s->current->newRelocation();
	failure = s->current->newRelocation();
	parseCondition(s, success, failure);

	s->current->fixRelocation(failure);
	s->current->thread(new caosAssert());
	s->current->fixRelocation(success);
}

void parseREPS::operator() (class caosScript *s, class Dialect *curD) {
			int exit = s->current->newRelocation();

			s->v->exp_dialect->parseOne(s); // repcount
			int entry = s->current->getNextIndex();
			s->current->thread(new caosREPS(exit));

			Dialect d;
			REPE r;
			d.delegates = s->v->cmd_dialect->delegates;
			d.delegates["repe"] = &r;

			d.doParse(s);
			s->current->thread(new caosJMP(entry));
			s->current->fixRelocation(exit);
		}

void EVER::operator() (class caosScript *s, class Dialect *curD) {
	s->current->thread(new caosJMP(exit));
	curD->stop = true;
}

void UNTL::operator() (class caosScript *s, class Dialect *curD) {
	parseCondition(s, exit, entry);
	curD->stop = true;
}

void parseLOOP::operator() (class caosScript *s, class Dialect *curD) {
	int exit = s->current->newRelocation();
	int entry = s->current->getNextIndex();

	Dialect d;
	EVER ever(entry); UNTL untl(entry, exit);
	d.delegates = s->v->cmd_dialect->delegates;
	d.delegates["ever"] = &ever;
	d.delegates["untl"] = &untl;

	d.doParse(s);
	s->current->fixRelocation(exit);
}

void parseGSUB::operator() (class caosScript *s, class Dialect *curD) {
	token *t = getToken(TOK_WORD);
	std::string label = t->word;
	int targ = s->current->gsub[label];
	if (!targ) {
		targ = s->current->newRelocation();
		s->current->gsub[label] = targ;
	}
	s->current->thread(new caosGSUB(targ));
}

void parseSUBR::operator() (class caosScript *s, class Dialect *curD) {
	s->current->thread(new caosSTOP());

	token *t = getToken(TOK_WORD);
	std::string label = t->word;
	int r = s->current->gsub[label];
	if (r)
		s->current->fixRelocation(r);
	s->current->gsub[label] = s->current->getNextIndex();
}

void ENUMhelper::operator() (class caosScript *s, class Dialect *curD) {
	(p)(s, curD);
	int exit = s->current->newRelocation();
	int entry = s->current->getNextIndex();
	s->current->thread(new caosENUM_POP(exit));

	Dialect d;
	NEXT n;
	d.delegates = s->v->cmd_dialect->delegates;
	d.delegates["next"] = &n;

	d.doParse(s);
	s->current->thread(new caosJMP(entry));
	s->current->fixRelocation(exit);
}


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
	while(argc--)
		s->v->exp_dialect->doParse(s);
	s->current->thread(new simpleCaosOp(handler, cd));
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
	p(s, this);
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

	
void registerDelegates() {
	registerAutoDelegates();
	map<std::string, Variant *>::iterator i = variants.begin();
	while (i != variants.end()) {
		Variant *v = i->second;
		const cmdinfo *p = v->cmds;
		while (p->key) {
			v->keyref[std::string(p->key)] = p;
			p++;
		}
		i++;
	}
}
/* vim: set noet: */
