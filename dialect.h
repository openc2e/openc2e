#ifndef DIALECT_H
#define DIALECT_H 1

#include "token.h"
//#include "caosScript.h"
#include "cmddata.h"
#include <map>

class script;
class caosScript;

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
		DefaultParser(const cmdinfo *i) :
			handler(i->handler), cd(i) {}
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
	std::string name;
	Dialect *cmd_dialect, *exp_dialect;
	const cmdinfo *cmds;
	std::map<std::string, const cmdinfo *> keyref;

	Variant(const char *n) : name(n) {}
	Variant(const std::string &n) : name(n) {}
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

// XXX: these really don't belong here

void parseCondition(caosScript *s, int success, int failure);

class DoifParser : public parseDelegate {
	protected:
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD);
};
		
class AssertParser : public parseDelegate {
	protected:
	public:
		virtual void operator()(class caosScript *s, class Dialect *curD);
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
		void operator() (class caosScript *s, class Dialect *curD);
};

class EVER : public parseDelegate {
	protected:
		int exit;
	public:
		EVER(int exit_) : exit(exit_) {}
		void operator() (class caosScript *s, class Dialect *curD);
};

class UNTL : public parseDelegate {
	protected:
		int entry, exit;
	public:
		UNTL(int en, int ex) : entry(en), exit(ex) {}
		void operator() (class caosScript *s, class Dialect *curD);
};

class parseLOOP : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) ;
};


class parseGSUB : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) ;
};

class parseSUBR : public parseDelegate {
	public:
		void operator() (class caosScript *s, class Dialect *curD) ;

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
		ENUMhelper(const cmdinfo *i) :
			p(i) {}

		void operator() (class caosScript *s, class Dialect *curD) ;
};

class ExprDialect : public OneShotDialect {
	public:
		void handleToken(caosScript *s, token *t);
};

void registerDelegates();

#endif

/* vim: set noet: */
