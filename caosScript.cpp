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

#include "bytecode.h"
#include "cmddata.h"
#include "exceptions.h"
#include "caosVM.h"
#include "openc2e.h"
#include "World.h"
#include "token.h"
#include "dialect.h"
#include "lex.yy.h"
#undef yyFlexLexer // flex/C++ is horrrrible I should use the C interface instead probably
#include "lex.c2.h"
#include "lexutil.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

using std::string;

class unexpectedEOIexception { };

script::~script() {
}

// resolve relocations into fixed addresses
void script::link() {
	ops.push_back(caosOp(CAOS_STOP, 0, -1));
	assert(!linked);
//	std::cout << "Pre-link:" << std::endl << dump();
	// check relocations
	for (unsigned int i = 1; i < relocations.size(); i++) {
		// handle relocations-to-relocations
		int p = relocations[i];
		while (p < 0)
			p = relocations[-p];
		relocations[i] = p;
	}
	for (unsigned int i = 0; i < ops.size(); i++) {
		if (op_is_relocatable(ops[i].opcode) && ops[i].argument < 0)
			ops[i].argument = relocations[-ops[i].argument];
	}
	linked = true;
//	std::cout << "Post-link:" << std::endl << dump();
	relocations.clear();
}

script::script(const Dialect *v, const std::string &fn)
	: fmly(-1), gnus(-1), spcs(-1), scrp(-1),
		dialect(v), filename(fn)
{
	// advance past reserved index 0
	ops.push_back(caosOp(CAOS_NOP, 0, -1));
	relocations.push_back(0);
	linked = false;
}
	
script::script(const Dialect *v, const std::string &fn,
		int fmly_, int gnus_, int spcs_, int scrp_)
	: fmly(fmly_), gnus(gnus_), spcs(spcs_), scrp(scrp_),
		dialect(v), filename(fn)
{
	ops.push_back(caosOp(CAOS_NOP, 0, -1));
	relocations.push_back(0);
	linked = false;
}

std::string script::dump() {
	std::ostringstream oss;
	oss << "Relocations:" << std::endl;
	for (unsigned int i = 1; i < relocations.size(); i++) {
		oss << boost::format("%08d -> %08d") % i % relocations[i]
			<< std::endl;
	}
	oss << "Code:" << std::endl;
	for (unsigned int i = 0; i < ops.size(); i++) {
		oss << boost::format("%08d: ") % i;
		oss << dumpOp(ops[i]);
		oss << std::endl;
	}
	return oss.str();
}

caosScript::caosScript(const std::string &dialect, const std::string &fn) {
	d = dialects[dialect].get();
	if (!d)
		throw parseException(std::string("Unknown dialect ") + dialect);
	current = installer = shared_ptr<script> (new script(d, fn));
	filename = fn;
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

void caosScript::installInstallScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short eventid) {
	assert((d->name == "c1") || (d->name == "c2"));

	installer->fmly = family;
	installer->gnus = genus;
	installer->spcs = species;
	installer->scrp = eventid;
	
	world.scriptorium.addScript(installer->fmly, installer->gnus, installer->spcs, installer->scrp, installer);
}

// parser states
enum {
	ST_INSTALLER,
	ST_BODY,
	ST_REMOVAL,
	ST_DOIF,
	ST_ENUM,
	ST_LOOP,
	ST_REPS,
	ST_INVALID
};

struct doifinfo {
	int failreloc;
	int donereloc;
};


token *caosScript::tokenPeek() {
	if ((size_t)curindex >= tokens->size())
		return NULL;
	return &(*tokens)[curindex];
}

token *caosScript::getToken(toktype expected) {
	token *t = tokenPeek();
	token dummy;
	token &r = (t ? *t : dummy);

	errindex = curindex;

	if (expected != ANYTOKEN && r.type() != expected)
		r.unexpected();

	curindex++;

	return t;
}

void caosScript::putBackToken(token *) {
	curindex--;
	errindex = curindex - 1; // curindex refers to the /next/ token to be parsed
							 // so make sure we refer to the token before it
}

void caosScript::parse(std::istream &in) {
	assert(!tokens);
	// run the token parser
	{
		extern int lex_lineno;
		extern bool using_c2;
		using_c2 = (d->name == "c1" || d->name == "c2");
		lexreset();
		boost::scoped_ptr<FlexLexer> l(
				using_c2 	? (FlexLexer *)new c2FlexLexer()
							: (FlexLexer *)new c2eFlexLexer()
		);
		l->yyrestart(&in);

		tokens = shared_ptr<std::vector<token> >(new std::vector<token>());
		while (l->yylex()) {
			tokens->push_back(lasttok);
			tokens->back().lineno = lex_lineno;
			tokens->back().index  = tokens->size() - 1;
		}
		tokens->push_back(token()); // tokens default to being EOI tokens
		tokens->back().lineno = lex_lineno;
		tokens->back().index  = tokens->size() - 1;
	}
	curindex = errindex = traceindex = 0;

	try {
		parseloop(ST_INSTALLER, NULL);

		std::ostringstream oss;
		shared_ptr<std::vector<toktrace> > tokinfo(new std::vector<toktrace>());
		for (size_t p = 0; p < tokens->size(); p++) {
			std::string tok = (*tokens)[p].format();
			int len = tok.size();
			if (len > 65535) {
				errindex = p;
				throw parseException("Overlong token");
			}
			oss << tok << " ";
			tokinfo->push_back(toktrace(len, (*tokens)[p].lineno));
		}
		std::string code = oss.str();
		installer->code = code;
		installer->tokinfo = tokinfo;
		installer->link();
		if (removal) {
			removal->link();
			removal->tokinfo = tokinfo;
			removal->code = code;
		}
		std::vector<shared_ptr<script> >::iterator i = scripts.begin();
		while (i != scripts.end()) {
			(*i)->tokinfo = tokinfo;
			(*i)->code = code;
			(*i++)->link();
		}
	} catch (parseException &e) {
		e.filename = filename;
		if (!tokens)
			throw;
		if (errindex < 0 || (size_t)errindex >= tokens->size())
			throw;
		e.lineno = (*tokens)[errindex].lineno;
		e.context = boost::shared_ptr<std::vector<token> >(new std::vector<token>());
		/* We'd like to capture N tokens on each side of the target, but
		 * if we can't get all those from one side, get it from the other.
		 */
		int contextlen = 5;
		int leftct = contextlen;
		int rightct = contextlen;

		if (errindex < leftct) {
			rightct += leftct - errindex;
			leftct = errindex;
		}
		if ((size_t)(errindex + rightct + 1) >= tokens->size()) {
			int overflow = errindex + rightct + 1 - tokens->size();
			rightct -= overflow;
			while (overflow > 0 && errindex > leftct) {
				overflow--;
				leftct++;
			}
		}
		assert(leftct >= 0 && rightct >= 0 && errindex >= leftct && (size_t)(errindex + rightct) < tokens->size());

		if (errindex - leftct != 0) {
			e.context->push_back(token());
			e.context->back().payload = std::string("...");
		}

		for (int i = errindex - leftct; i < errindex + rightct; i++) {
			e.context->push_back((*tokens)[i]);
		}
		if (errindex + rightct + 1 < tokens->size()) {
			e.context->push_back(token());
			e.context->back().payload = std::string("...");
		}
		e.ctxoffset = errindex;
		throw;
	}
}

const cmdinfo *caosScript::readCommand(token *t, const std::string &prefix) {
	std::string fullname = prefix + t->word();
	const cmdinfo *ci = d->find_command(fullname.c_str());
	// See if there'{s a subcommand namespace
	token *t2 = NULL;
	try {
		t2 = getToken(TOK_WORD);
		if (!t2 || t2->type() != TOK_WORD)
			throw parseException("dummy");
		return readCommand(t2, fullname + " ");
	} catch (parseException &e) {
		if (ci->argtypes && ci->argtypes[0] == CI_SUBCOMMAND)
			throw;
		if (t2)
			putBackToken(t2);
		return ci;
	}
}

void caosScript::emitOp(opcode_t op, int argument) {
	current->ops.push_back(caosOp(op, argument, traceindex));
}

void caosScript::readExpr(const enum ci_type *argp) {
	// TODO: typecheck
	int saved_trace = traceindex;
	if (!argp) throw parseException("Internal error: null argp");
	while (*argp != CI_END) {
		if (*argp == CI_BAREWORD) {
			token *t = getToken(ANYTOKEN);
			switch (t->type()) {
				case TOK_WORD:
					current->consts.push_back(t->word());
					break;
				case TOK_CONST:
					if (t->constval().getType() != STRING)
						t->unexpected();
					current->consts.push_back(t->constval());
					break;
				default:
					t->unexpected();
			}
			emitOp(CAOS_CONST, current->consts.size() - 1);
			argp++;
			continue;
		}
		token *t = getToken(ANYTOKEN);
		traceindex = errindex;
		switch (t->type()) {
			case EOI: throw parseException("Unexpected end of input");
			case TOK_CONST:
				{
					if (t->constval().getType() == INTEGER) {
						int val = t->constval().getInt();
						// small values can be immediates
						if (val >= -(1 << 24) && val < (1 << 24)) {
							emitOp(CAOS_CONSTINT, val);
							break;
						}
					}
					// big values must be put in the constant table
					current->consts.push_back(t->constval());
					emitOp(CAOS_CONST, current->consts.size() - 1);
					break;
				}
			case TOK_BYTESTR:
				{
					current->bytestrs.push_back(t->bytestr());
					emitOp(CAOS_BYTESTR, current->bytestrs.size() - 1);
					break;
				}
			case TOK_WORD:
				{
					if (t->word() == "face") {
						// horrible hack :(
						// this might confuse people, hmm. XXX: do this without frobbing the
						// token buffer
						if (*argp == CI_NUMERIC)
							t->payload = std::string("face int");
						else
							t->payload = std::string("face string");
					}
					// vaxx, mvxx, ovxx
					if (t->word().size() == 4
						&&	((t->word()[1] == 'v' && (t->word()[0] == 'o' || t->word()[0] == 'm'))
								|| (t->word()[0] == 'v' && t->word()[1] == 'a'))
						&&  isdigit(t->word()[2]) && isdigit(t->word()[3])) {
						int vidx = atoi(t->word().c_str() + 2);
						opcode_t op;
						switch(t->word()[0]) {
							case 'v':
								op = CAOS_VAXX;
								break;
							case 'o':
								op = CAOS_OVXX;
								break;
							case 'm':
								op = CAOS_MVXX;
								break;
							default:
								assert(0 && "UNREACHABLE");
						}
						emitOp(op, vidx);
						break;
					}
					// obvx
					if (t->word().size() == 4 && !strncmp(t->word().c_str(), "obv", 3) && isdigit(t->word()[3])) {
						emitOp(CAOS_OVXX, atoi(t->word().c_str() + 3));
						break;
					}
					// varx
					if (t->word().size() == 4 && !strncmp(t->word().c_str(), "var", 3) && isdigit(t->word()[3])) {
						emitOp(CAOS_VAXX, atoi(t->word().c_str() + 3));
						break;
					}
					const cmdinfo *ci = readCommand(t, std::string("expr "));
					if (ci->argc)
						readExpr(ci->argtypes);
					emitOp(CAOS_CMD, d->cmd_index(ci));
					break;
				}
			default: throw parseException("Unexpected token");
		}
		argp++;
	}
	traceindex = saved_trace;
}

int caosScript::readCond() {
	token *t = getToken(TOK_WORD);
	typedef struct { char *n; int cnd; } cond_entry;
	const static cond_entry conds[] = {
		{ "eq", CEQ },
		{ "gt", CGT },
		{ "ge", CGE },
		{ "lt", CLT },
		{ "le", CLE },
		{ "ne", CNE },
		{ "bt", CBT },
		{ "bf", CBF },
		{ NULL, 0 }
	};

	const cond_entry *c = conds;
	while (c->n != NULL) {
		if (t->word() == c->n)
			return c->cnd;
		c++;
	}
	throw parseException(std::string("Unexpected non-condition word: ") + t->word());
}

void caosScript::parseCondition() {
	const static ci_type onearg[] = { CI_ANYVALUE, CI_END };
	emitOp(CAOS_CONSTINT, 1);

	bool nextIsAnd = true;
	while (1) {
		readExpr(onearg);
		int cond = readCond();
		readExpr(onearg);
		emitOp(CAOS_COND, cond | (nextIsAnd ? CAND : COR));

		token *peek = tokenPeek();
		if (!peek) break;
		if (peek->type() != TOK_WORD) break;
		if (peek->word() == "and") {
			getToken();
			nextIsAnd = true;
		} else if (peek->word() == "or") {
			getToken();
			nextIsAnd = false;
		} else break;
	}
}
	
void caosScript::parseloop(int state, void *info) {
	token *t;
	while ((t = getToken(ANYTOKEN))) {
		traceindex = errindex;
		if (t->type() == EOI) {
			switch (state) {
				case ST_INSTALLER:
				case ST_BODY:
				case ST_REMOVAL:
					return;
				default:
					throw parseException("Unexpected end of input");
			}
		}
		if (t->type() != TOK_WORD) {
			throw parseException("Unexpected non-word token");
		}
		if (t->word() == "scrp") {
			if (state != ST_INSTALLER)
				throw parseException("Unexpected SCRP");
			state = ST_BODY;
			int bits[4];
			for (int i = 0; i < 4; i++) {
				caosVar val = getToken(TOK_CONST)->constval();
				if (!val.getType() == INTEGER)
					throw parseException("Expected integer constant");
				bits[i] = val.getInt();
			}
			int fmly = bits[0];
			int gnus = bits[1];
			int spcs = bits[2];
			int scrp = bits[3];
			scripts.push_back(shared_ptr<script>(new script(d, filename, fmly, gnus, spcs, scrp)));
			current = scripts.back();
		} else if (t->word() == "rscr") {
			if (state == ST_INSTALLER || state == ST_BODY || state == ST_REMOVAL)
				state = ST_REMOVAL;
			else
				throw parseException("Unexpected RSCR");
			if (!removal)
				removal = shared_ptr<script>(new script(d, filename));
			current = removal;
		} else if (t->word() == "iscr") {
			if (state == ST_INSTALLER || state == ST_BODY || state == ST_REMOVAL)
				state = ST_INSTALLER;
			else
				throw parseException("Unexpected RSCR");
			current = installer;
		} else if (t->word() == "endm") {
			if (state == ST_BODY) {
				state = ST_INSTALLER;
				current = installer;
			} else {
				// I hate you. Die in a fire.
				emitOp(CAOS_STOP, 0);
				putBackToken(t);
				return;
			}
			// No we will not emit c_ENDM() thankyouverymuch

		} else if (t->word() == "enum"
				|| t->word() == "esee"
				|| t->word() == "etch"
				|| t->word() == "epas"
				|| t->word() == "econ") {
			int nextreloc = current->newRelocation();
			// XXX: copypasta
			const cmdinfo *ci = readCommand(t, std::string("cmd "));
			if (ci->argc) {
				if (!ci->argtypes)
					std::cerr << "Missing argtypes for command " << t->word() << "; probably unimplemented." << std::endl;
				readExpr(ci->argtypes);
			}
			emitOp(CAOS_CMD, d->cmd_index(ci));
			emitOp(CAOS_JMP, nextreloc);
			int startp = current->getNextIndex();
			parseloop(ST_ENUM, NULL);
			current->fixRelocation(nextreloc);
			emitOp(CAOS_ENUMPOP, startp);
		} else if (t->word() == "next") {
			if (state != ST_ENUM) {
				throw parseException("Unexpected NEXT");
			}
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd next")));
			return;

		} else if (t->word() == "subr") {
			// Yes, this will work in a doif or whatever. This is UB, it may
			// be made to not compile later.
			t = getToken(TOK_WORD);
			std::string label = t->word();
			emitOp(CAOS_STOP, 0);
			current->affixLabel(label);
		} else if (t->word() == "gsub") {
			t = getToken(TOK_WORD);
			std::string label = t->word();
			emitOp(CAOS_GSUB, current->getLabel(label));

		} else if (t->word() == "loop") {
			int loop = current->getNextIndex();
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd loop")));
			parseloop(ST_LOOP, (void *)&loop);			
		} else if (t->word() == "untl") {
			if (state != ST_LOOP)
				throw parseException("Unexpected UNTL");
			// TODO: zerocost logic inversion - do in c_UNTL()?
			int loop = *(int *)info;
			int out  = current->newRelocation();
			parseCondition();
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd untl")));
			emitOp(CAOS_CJMP, out);
			emitOp(CAOS_JMP, loop);
			current->fixRelocation(out);
			return;
		} else if (t->word() == "ever") {
			if (state != ST_LOOP)
				throw parseException("Unexpected EVER");
			int loop = *(int *)info;
			emitOp(CAOS_JMP, loop);
			return;

		} else if (t->word() == "reps") {
			const static ci_type types[] = { CI_NUMERIC, CI_END };
			readExpr(types);
			int loop = current->getNextIndex();
			parseloop(ST_REPS, (void *)&loop);
		} else if (t->word() == "repe") {
			if (state != ST_REPS)
				throw parseException("Unexpected repe");
			emitOp(CAOS_DECJNZ, *(int *)info);
			return;

		} else if (t->word() == "doif") {
			struct doifinfo di;
			di.donereloc = current->newRelocation();
			di.failreloc = current->newRelocation();
			int okreloc = current->newRelocation();

			parseCondition();
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd doif")));
			emitOp(CAOS_CJMP, okreloc);
			emitOp(CAOS_JMP, di.failreloc);
			current->fixRelocation(okreloc);
			parseloop(ST_DOIF, (void *)&di);
			if (di.failreloc)
				current->fixRelocation(di.failreloc);
			current->fixRelocation(di.donereloc);
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd endi")));
		} else if (t->word() == "elif") {
			if (state != ST_DOIF) {
				// XXX this is horrible
				t->payload = std::string("doif");
				continue;
			}
			struct doifinfo *di = (struct doifinfo *)info;
			int okreloc = current->newRelocation();

			emitOp(CAOS_JMP, di->donereloc);
			current->fixRelocation(di->failreloc);
			di->failreloc = current->newRelocation();
			parseCondition();
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd elif")));
			emitOp(CAOS_CJMP, okreloc);
			emitOp(CAOS_JMP, di->failreloc);
			current->fixRelocation(okreloc);
			parseloop(ST_DOIF, info);
			return;
		} else if (t->word() == "else") {
			if (state != ST_DOIF)
				throw parseException("Unexpected ELSE");
			struct doifinfo *di = (struct doifinfo *)info;
			if (!di->failreloc)
				throw parseException("Duplicate ELSE");
			emitOp(CAOS_JMP, di->donereloc);
			current->fixRelocation(di->failreloc);
			di->failreloc = 0;
			emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd else")));
		} else if (t->word() == "endi") {
			if (state != ST_DOIF) {
				emitOp(CAOS_DIE, -1);
				continue;
			}
			return;
		} else {
			if (t->word() == "dbg:") {
				token *t2 = tokenPeek();
				if (t2 && t2->type() == TOK_WORD && t2->word() == "asrt") {
					getToken(TOK_WORD);
					emitOp(CAOS_CONSTINT, 1);
					parseCondition();
					int endreloc = current->newRelocation();
					emitOp(CAOS_CJMP, endreloc);
					emitOp(CAOS_CMD, d->cmd_index(d->find_command("cmd dbg: asrt")));
					current->fixRelocation(endreloc);
					continue;
				}
			}
				
			const cmdinfo *ci = readCommand(t, std::string("cmd "));
			if (ci->argc) {
				if (!ci->argtypes)
					std::cerr << "Missing argtypes for command " << t->word() << "; probably unimplemented." << std::endl;
				readExpr(ci->argtypes);
			}
			emitOp(CAOS_CMD, d->cmd_index(ci));
		}
	}
}
			

/* vim: set noet: */
