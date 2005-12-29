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

void script::thread(caosOp *op) {
	assert (!op->owned && !linked);
	op->owned = true;
	op->index = allOps.size();
	allOps.push_back(op);
}

script::~script() {
	std::vector<class caosOp *>::iterator i = allOps.begin();
	while (i != allOps.end())
		delete *i++;
	allOps.clear();
	relocations.clear();
	gsub.clear();
}

void script::link() {
	thread(new caosSTOP());
	assert(!linked);
//	std::cout << "Pre-link:" << std::endl << dump();
	// check relocations
	for (int i = 1; i < relocations.size(); i++) {
		// handle relocations-to-relocations
		int p = relocations[i];
		while (p < 0)
			p = relocations[-p];
		relocations[i] = p;
	}
	for (int i = 0; i < allOps.size(); i++) {
		allOps[i]->relocate(relocations);
	}
	linked = true;
//	std::cout << "Post-link:" << std::endl << dump();
}

script::script(const Variant *v, const std::string &fn)
	: fmly(-1), gnus(-1), spcs(-1), scrp(-1),
	  variant(v), filename(fn)
{
	allOps.push_back(new caosNoop());
	relocations.push_back(0);
	linked = false;
}
	
script::script(const Variant *v, const std::string &fn,
		int fmly_, int gnus_, int spcs_, int scrp_)
	: fmly(fmly_), gnus(gnus_), spcs(spcs_), scrp(scrp_),
	  variant(v), filename(fn)
{
	allOps.push_back(new caosNoop());
	relocations.push_back(0);
	linked = false;
}

std::string script::dump() {
	std::ostringstream oss;
	oss << "Relocations:" << std::endl;
	for (int i = 1; i < relocations.size(); i++) {
		char buf[32];
		sprintf(buf, "%08d -> %08d", i, relocations[i]);
		oss << buf << std::endl;
	}
	oss << "Code:" << std::endl;
	for (int i = 0; i < allOps.size(); i++) {
		char buf[16];
		sprintf(buf, "%08d: ", i);
		oss << buf;
		oss << allOps[i]->dump();
		oss << std::endl;
	}
	return oss.str();
}

#if 0
// for gdb
static int dump_out(shared_ptr<script> s) {
	std::cerr << s->dump() << std::endl;
}
#endif
	

class ENDM : public parseDelegate {
	public:
		void operator() (class caosScript *s, Dialect *curD) {
			curD->stop = true;
		}
};

class BaseDialect : public Dialect {
	public:
		BaseDialect(caosScript *s) {
			delegates = s->v->cmd_dialect->delegates;
		}

		virtual void handleToken(class caosScript *s, token *t) {
			if (t->type == TOK_WORD) {
				if (t->word == "rscr") {
					if (s->removal)
						throw parseException("multiple rscr not allowed");
					s->current = s->removal = shared_ptr<script>(new script(s->v, s->filename));
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
					d.delegates = s->v->cmd_dialect->delegates;
					d.delegates["endm"] = &endm;
					shared_ptr<script> scr = shared_ptr<script>(
									new script(s->v, s->filename,
										fmly.constval.getInt(),
										gnus.constval.getInt(),
										spcs.constval.getInt(),
										scrp.constval.getInt()));
					s->current = scr;
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

caosScript::caosScript(const std::string &variant, const std::string &fn) {
	v = variants[variant];
	if (!v)
		throw caosException(std::string("Unknown variant ") + variant);
	current = installer = shared_ptr<script> (new script(v, fn));
	filename = fn;
}

void caosScript::parse(std::istream &in) {

	yyrestart(&in);

	BaseDialect d(this);
	d.doParse(this);

	installer->link();
	if (removal)
		removal->link();
	std::vector<shared_ptr<script> >::iterator i = scripts.begin();
	while (i != scripts.end()) {
		(*i)->link();
		i++;
	}
}

caosScript::~caosScript() {
	// Nothing to do, yay shared_ptr!
}

void caosScript::installScripts() {
	std::vector<shared_ptr<script> >::iterator i = scripts.begin();
	while (i != scripts.end()) {
		shared_ptr<script> s = *i;
		world.scriptorium.addScript(s->fmly, s->gnus, s->spcs, s->scrp, s);
		i++;
	}
}

/* vim: set noet: */
