#ifndef DIALECT_H
#define DIALECT_H 1

#include "token.h"
#include "bytecode.h"
#include "caosScript.h"
#include "cmddata.h"
#include <map>

class parseDelegate {
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD) {
			(*this)(s, curD, CI_OTHER);
		}
		virtual void operator()(class caosScript *s, class Dialect *curD, enum ci_type expect_type) {
			(*this)(s, curD);
		}
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
	protected:
		enum ci_type expect; // THIS IS A HORRIBLE HACK
		                     // THANKS A LOT CL
							 // YOU RUINED MY BEAUTIFUL PARSER DESIGN
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
		void set_expect(enum ci_type e) { expect = e; }
};

struct Variant {
	Dialect *cmd_dialect, *exp_dialect;
	const cmdinfo *cmds;
};

extern std::map<std::string, Variant *> variants;

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

void parseCondition(caosScript *s, int success, int failure);

class DoifDialect : public Dialect {
	protected:
		int success, failure, exit;
	public:
		DoifDialect(caosScript *scr, int s, int f, int e)
			: success(s), failure(f), exit(e) {
				delegates = scr->v->cmd_dialect->delegates; // XXX
			}
		void handleToken(class caosScript *s, token *t); 
};

class DoifParser : public parseDelegate {
	protected:
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD) {
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
};
		
class AssertParser : public parseDelegate {
	protected:
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD) {
			int success, failure;
			success = s->current->newRelocation();
			failure = s->current->newRelocation();
			parseCondition(s, success, failure);

			s->current->fixRelocation(failure);
			s->current->thread(new caosAssert());
			s->current->fixRelocation(success);
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
};

class EVER : public parseDelegate {
	protected:
		int exit;
	public:
		EVER(int exit_) : exit(exit_) {}
		void operator() (class caosScript *s, class Dialect *curD) {
			s->current->thread(new caosJMP(exit));
			curD->stop = true;
		}
};

class UNTL : public parseDelegate {
	protected:
		int entry, exit;
	public:
		UNTL(int en, int ex) : entry(en), exit(ex) {}
		void operator() (class caosScript *s, class Dialect *curD) {
			parseCondition(s, exit, entry);
			curD->stop = true;
		}
};

class parseLOOP : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
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
};


class parseGSUB : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			token *t = getToken(TOK_WORD);
			std::string label = t->word;
			int targ = s->current->gsub[label];
			if (!targ) {
				targ = s->current->newRelocation();
				s->current->gsub[label] = targ;
			}
			s->current->thread(new caosGSUB(targ));
		}
};

class parseSUBR : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) {
			s->current->thread(new caosSTOP());

			token *t = getToken(TOK_WORD);
			std::string label = t->word;
			int r = s->current->gsub[label];
			if (r)
				s->current->fixRelocation(r);
			s->current->gsub[label] = s->current->getNextIndex();
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
};
class ExprDialect : public OneShotDialect {
	protected:
	public:
		void handleToken(caosScript *s, token *t);
};

void registerDelegates();

class FACEhelper : public parseDelegate {
	/* I hate you, CL --bd */
	public:
		void operator() (class caosScript *s, class Dialect *curD, enum ci_type t) {
			s->current->thread(new caosFACE(t));
		}
};

#endif

/* vim: set noet: */
