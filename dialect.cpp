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

Dialect *cmd_dialect, *exp_dialect;

void parseCondition(caosScript *s, caosOp *success, caosOp *failure) {
	while(1) {
		exp_dialect->doParse(s);
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
		exp_dialect->doParse(s);
		

		/*
		 * If the next bind is or, we jump to success.
		 * Otherwise, we negate and jump to failure.
		 * The last item is always considered or-ed.
		 */
		int isOr = 1;
		int isLast = 0;

		struct token *peek = tokenPeek();
		if (!peek)
			throw parseException("unexpected eoi");
		if (peek->type == TOK_WORD) {
			if (peek->word == "and") {
				getToken();
				isOr = 0;
			} else if (peek->word == "or")
				getToken();
			else
				isLast = 1;
		}

		caosOp *jumpTarget = isOr ? success : failure;
		if (!isOr) compar = ~compar & CMASK;
		
		s->current->thread(new caosCond(compar, jumpTarget));
		if (isLast) break;
	}
	s->current->last->setSuccessor(failure);
}

void DefaultParser::operator()(class caosScript *s, class Dialect *curD) {
	int argc = cmds[idx].argc;
	while(argc--)
		exp_dialect->doParse(s);
	s->current->thread(new simpleCaosOp(handler, idx));
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
		throw parseException(std::string("no delegate for word ") + word);
	parseDelegate &p = *delegates[word];
	p(s, this);
}

class ConstOp : public caosOp {
	protected:
		caosVar constVal;
	public:
		virtual void execute(caosVM *vm) {
			vm->valueStack.push_back(constVal);
			caosOp::execute(vm);
		}

		ConstOp(const caosVar &val) {
			constVal = val;
		}
};

class opVAxx : public caosOp {
	protected:
		const int index;
	public:
		opVAxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			vm->valueStack.push_back(&vm->var[index]);
		}
};

class opOVxx : public caosOp {
	protected:
		const int index;
	public:
		opOVxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			caos_assert(vm->targ);
			vm->valueStack.push_back(&vm->targ->var[index]);
		}
};

class opMVxx : public caosOp {
	protected:
		const int index;
	public:
		opMVxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			caos_assert(vm->owner);
			vm->valueStack.push_back(&vm->owner->var[index]);
		}
};

class opBytestr : public caosOp {
	protected:
		std::vector<unsigned int> bytestr;
	public:
		opBytestr(const std::vector<unsigned int> &bs) : bytestr(bs) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			vm->valueStack.push_back(bytestr);
		}
};


class ExprDialect : public OneShotDialect {
	public:
		void handleToken(caosScript *s, token *t) {
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
};

void DoifDialect::handleToken(class caosScript *s, token *t) {
	if (t->type == TOK_WORD) {
		if (t->word == "endi") {
			if (failure) // we don't have an else clause
				s->current->thread(failure);
			s->current->thread(exit);
			stop = true;
			return;
		}
		if (t->word == "else") {
			if (!failure)
				throw new parseException("double else clause is forbidden");
			s->current->thread(exit);
			s->current->last = failure;
			failure = NULL;
			return;
		}
		if (t->word == "elif") {
			// emuluate an else-doif-endi block
			if (!failure)
				throw new parseException("double else clause is forbidden");
			s->current->thread(exit);
			s->current->last = failure;
			failure = NULL;
			DoifParser dip;
			dip(s, this);
			s->current->thread(exit);
			stop = true;
			return;
		}
	}
	Dialect::handleToken(s, t);
}

	
void registerDelegates() {
	cmd_dialect = new Dialect();
	exp_dialect = new ExprDialect();
	registerAutoDelegates();
}
/* vim: set noet: */
