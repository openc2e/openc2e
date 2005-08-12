/*
 *  caosScript.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed May 26 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "exceptions.h"
#include "caosVM.h"
#include "openc2e.h"
#include "World.h"
#include "token.h"
#include "dialect.h"
#include "lex.yy.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>

using std::string;

class unexpectedEOIexception { };

void script::addOp(caosOp *op) {
	if (op->owned) return;
	op->owned = true;
	allOps.push_back(op);
}

void script::thread(caosOp *op) {
	addOp(op);
	last->setSuccessor(op);
	last = op;
}

script::~script() {
	std::vector<class caosOp *>::iterator i = allOps.begin();
	while (i != allOps.end())
		delete *i++;
}


script::script(const std::string &fn) {
	filename = fn;
	entry = last = new caosNoop();
	allOps.push_back(entry);
}


class ENDM : public parseDelegate {
	public:
		void operator() (class caosScript *s, Dialect *curD) {
			curD->stop = true;
		}
};

class BaseDialect : public Dialect {
	public:
		BaseDialect() {
			delegates = cmd_dialect->delegates;
		}

		virtual void handleToken(class caosScript *s, token *t) {
			if (t->type == TOK_WORD) {
				if (t->word == "rscr") {
					if (s->removal)
						throw parseException("multiple rscr not allowed");
					s->current = s->removal = new script(s->filename);
					s->removal->retain();
					return;
				}
				if (t->word == "scrp") {
					if (s->removal)
						throw parseException("scrp after rscr");
					token fmly = *getToken(TOK_CONST);
					if (!fmly.constval.hasInt())
					   throw parseException("classifier values must be ints");
					token gnus = *getToken(TOK_CONST);
					if (!gnus.constval.hasInt())
					   throw parseException("classifier values must be ints");
					token spcs = *getToken(TOK_CONST);
					if (!spcs.constval.hasInt())
					   throw parseException("classifier values must be ints");
					token scrp = *getToken(TOK_CONST);
					if (!scrp.constval.hasInt())
					   throw parseException("classifier values must be ints");
					token *next = tokenPeek();
					if (!next)
						throw parseException("unexpected end of input");
					if (next->type == TOK_WORD && next->word == "endm") {
						getToken();
						return;
					}
					Dialect d;
					ENDM endm;
					d.delegates = cmd_dialect->delegates;
					d.delegates["endm"] = &endm;
					struct residentScript scr(
							fmly.constval.getInt(),
							gnus.constval.getInt(),
							spcs.constval.getInt(),
							scrp.constval.getInt(),
							new script(s->filename));
					s->current = scr.s;
					d.doParse(s);
					s->current = s->installer;
					assert(s->current);
					s->scripts.push_back(scr);
					if (!d.stop)
					   throw parseException("expected endm");
					return;
				}
				Dialect::handleToken(s, t);
			} // if (t->type == TOK_WORD)
		}
};

caosScript::caosScript(const std::string &fn) {
	current = installer = new script(fn);
	current->retain();
	removal = NULL;
	filename = fn;
}

void caosScript::parse(std::istream &in) {

	yyrestart(&in);

	BaseDialect d;
	d.doParse(this);

}

caosScript::~caosScript() {
	installer->release();
	if (removal)
		removal->release();
	std::vector<residentScript>::iterator i = scripts.begin();
	while (i != scripts.end())
		i++->s->release();
}

void caosScript::installScripts() {
	std::vector<residentScript>::iterator i = scripts.begin();
	while (i != scripts.end()) {
		world.scriptorium.addScript(i->fmly, i->gnus, i->spcs, i->scrp, i->s);
		i++;
	}
}

/* vim: set noet: */
