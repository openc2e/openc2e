#ifndef DIALECT_H
#define DIALECT_H 1

#include "token.h"
#include "bytecode.h"
#include "caosScript.h"
#include "cmddata.h"
#include <map>

class parseDelegate {
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD) = 0;
		virtual ~parseDelegate() {}
};

class DefaultParser : public parseDelegate {
	protected:
		void (caosVM::*handler)();
		const cmdinfo *cd;
	public:
		DefaultParser(void (caosVM::*h)(), const cmdinfo *i) :
			handler(h), cd(i) {}
		virtual void operator()(class caosScript *s, class Dialect *curD);
};

class Dialect {
	public:
		bool stop;

		Dialect() : stop(false) {}
		
		std::map<std::string, parseDelegate *> delegates;
		virtual void doParse(class caosScript *s) {
			while (!stop && parseOne(s));
		}
		virtual bool parseOne(class caosScript *s);
		virtual void handleToken(class caosScript *s, token *t);
		virtual void eof() {};
		virtual ~Dialect() {};
};

struct Variant {
	Dialect *cmd_dialect, *exp_dialect;
};

extern map<std::string, Variant *> variants;

class OneShotDialect : public Dialect {
	public:
		void doParse(class caosScript *s) {
			Dialect::parseOne(s);
		}
		void eof() {
			throw caosException("unexpected EOF");
		}
};

// XXX: these don't really belong here

void parseCondition(caosScript *s, caosOp *success, caosOp *failure);

class DoifDialect : public Dialect {
	protected:
		caosOp *success, *failure, *exit;
	public:
		DoifDialect(caosScript *scr, caosOp *s, caosOp *f, caosOp *e)
			: success(s), failure(f), exit(e) {
				delegates = scr->v->cmd_dialect->delegates; // XXX
			}
		void handleToken(class caosScript *s, token *t); 
};

class DoifParser : public parseDelegate {
	protected:
		caosOp *success, *failure, *exit;
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD) {
			success = new caosNoop();
			failure = new caosNoop();
			exit = new caosNoop();
			s->current->addOp(success);
			s->current->addOp(failure);
			s->current->addOp(exit);
			
			parseCondition(s, success, failure);
			
			s->current->last = success;
			DoifDialect d(s, success, failure, exit);
			d.doParse(s);
		}
};
		
class NamespaceDelegate : public parseDelegate {
	public:
		OneShotDialect dialect;
		void operator() (class caosScript *s, class Dialect *curD) {
			dialect.parseOne(s);
		}
};

class REPE : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			curD->stop = true;
		}
};

class parseREPS : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			caosOp *exit = new caosNoop();
			s->current->addOp(exit);
			
			s->v->exp_dialect->parseOne(s); // repcount
			caosOp *entry = new caosREPS(exit);
			s->current->thread(entry);

			Dialect d;
			REPE r;
			d.delegates = s->v->cmd_dialect->delegates;
			d.delegates["repe"] = &r;

			d.doParse(s);
			s->current->last->setSuccessor(entry);
			s->current->last = exit;
		}
};

class EVER : public parseDelegate {
	protected:
		caosOp *exit;
	public:
		EVER(caosOp *exit_) : exit(exit_) {}
		void operator() (class caosScript *s, class Dialect *curD) {
			s->current->thread(exit);
			curD->stop = true;
		}
};

class UNTL : public parseDelegate {
	protected:
		caosOp *entry, *exit;
	public:
		UNTL(caosOp *en, caosOp *ex) : entry(en), exit(ex) {}
		void operator() (class caosScript *s, class Dialect *curD) {
			parseCondition(s, exit, entry);
			curD->stop = true;
		}
};

class parseLOOP : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			caosOp *exit = new caosNoop();
			s->current->addOp(exit);
			
			caosOp *entry = new caosNoop();
			s->current->thread(entry);

			Dialect d;
			EVER ever(entry); UNTL untl(entry, exit);
			d.delegates = s->v->cmd_dialect->delegates;
			d.delegates["ever"] = &ever;
			d.delegates["untl"] = &untl;

			d.doParse(s);
			// No need to thread - if we use UNTL, we _will_ go to either
			// entry or exit
			s->current->last = exit;
		}
};


class parseGSUB : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			token *t = getToken(TOK_WORD);
			std::string label = t->word;
			caosOp *targ = s->current->gsub[label];
			if (!targ) {
				targ = new caosNoop();
				s->current->addOp(targ);
				s->current->gsub[label] = targ;
			}
			s->current->thread(new caosGSUB(targ));
		}
};

class parseSUBR : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			token *t = getToken(TOK_WORD);
			std::string label = t->word;
			caosOp *targ = s->current->gsub[label];
			if (!targ) {
				targ = new caosNoop();
				s->current->addOp(targ);
				s->current->gsub[label] = targ;
			}
			s->current->last = targ;
		}
};

class NEXT : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			curD->stop = true;
		}
};

class ENUMhelper : public parseDelegate {
	protected:
		DefaultParser p;
	public:
		ENUMhelper(void (caosVM::*h)(), const cmdinfo *i) :
			p(h,i) {}

		void operator() (class caosScript *s, class Dialect *curD) {
			(p)(s, curD);
			caosOp *exit = new caosNoop();
			caosOp *entry = new caosENUM_POP(exit);
			s->current->thread(entry);
			s->current->addOp(exit);
			
			Dialect d;
			NEXT n;
			d.delegates = s->v->cmd_dialect->delegates;
			d.delegates["next"] = &n;
			
			d.doParse(s);
			s->current->thread(entry);
			s->current->last = exit;
		}
};
class ExprDialect : public OneShotDialect {
	public:
		void handleToken(caosScript *s, token *t);
};

void registerDelegates();

#endif

/* vim: set noet: */
