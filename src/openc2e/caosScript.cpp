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

#include "caosScript.h"

#include "Engine.h"
#include "Scriptorium.h"
#include "World.h"
#include "bytecode.h"
#include "caosVM.h"
#include "cmddata.h"
#include "common/Exception.h"
#include "common/readfile.h"
#include "dialect.h"
#include "fileformats/caoslexer.h"
#include "fileformats/caostoken.h"
#include "parseException.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fmt/core.h>
#include <iostream>
#include <memory>

using std::string;

class unexpectedEOIexception {};

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
	for (auto& op : ops) {
		if (op_is_relocatable(op.opcode) && op.argument < 0)
			op.argument = relocations[-op.argument];
	}
	linked = true;
	//	std::cout << "Post-link:" << std::endl << dump();
	relocations.clear();
}

script::script(const Dialect* v, const std::string& fn)
	: fmly(-1), gnus(-1), spcs(-1), scrp(-1),
	  dialect(v), filename(fn) {
	// advance past reserved index 0
	ops.push_back(caosOp(CAOS_NOP, 0, -1));
	relocations.push_back(0);
	memset(varRemap, 0xFF, 100);
	varUsed = 0;
	linked = false;
}

script::script(const Dialect* v, const std::string& fn,
	int fmly_, int gnus_, int spcs_, int scrp_)
	: fmly(fmly_), gnus(gnus_), spcs(spcs_), scrp(scrp_),
	  dialect(v), filename(fn) {
	ops.push_back(caosOp(CAOS_NOP, 0, -1));
	relocations.push_back(0);
	memset(varRemap, 0xFF, 100);
	varUsed = 0;
	linked = false;
}

std::string script::dump() {
	std::string buf = "Relocations:\n";
	for (unsigned int i = 1; i < relocations.size(); i++) {
		buf += fmt::format("{:08d} -> {:08d}\n", i, relocations[i]);
	}
	buf += "Code:\n";
	for (unsigned int i = 0; i < ops.size(); i++) {
		buf += fmt::format("{:08d}: {}\n", i, dumpOp(dialect, ops[i]));
	}
	return buf;
}

caosScript::caosScript(const std::string& dialect, const std::string& _filename) {
	enumdepth = 0;
	d = getDialectByName(dialect);
	if (!d)
		throw parseException(std::string("Unknown dialect ") + dialect);
	current = installer = std::shared_ptr<script>(new script(d, _filename));
	filename = _filename;
}

caosScript::~caosScript() {
	// Nothing to do, yay std::shared_ptr!
}

void caosScript::installScripts() {
	std::vector<std::shared_ptr<script> >::iterator i = scripts.begin();
	while (i != scripts.end()) {
		std::shared_ptr<script> s = *i;
		world.scriptorium->addScript(s->fmly, s->gnus, s->spcs, s->scrp, s);
		i++;
	}
}

void caosScript::installInstallScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short eventid) {
	assert((d->name == "c1") || (d->name == "c2"));

	installer->fmly = family;
	installer->gnus = genus;
	installer->spcs = species;
	installer->scrp = eventid;

	world.scriptorium->addScript(installer->fmly, installer->gnus, installer->spcs, installer->scrp, installer);
}

void caosScript::emitConst(const caosValue& v) {
	if (v.hasInt()) {
		int val = v.getInt();
		if (val >= -(1 << 24) && val < (1 << 24)) {
			emitOp(CAOS_CONSTINT, val);
			return;
		}
	}
	current->consts.push_back(v);
	emitOp(CAOS_CONST, current->consts.size() - 1);
}

// parser states
enum {
	ST_INSTALLER,
	ST_BODY,
	ST_REMOVAL,
	ST_DOIF,
	ST_ENUM,
	ST_ESCN,
	ST_LOOP,
	ST_REPS,
	ST_INVALID
};

struct doifinfo {
	int failreloc;
	int donereloc;
};

struct repsinfo {
	int jnzreloc;
	int loopidx;
};

caostoken* caosScript::tokenPeek() {
	while (true) {
		if ((size_t)curindex >= tokens->size()) {
			return NULL;
		}
		return &(*tokens)[curindex];
	}
}

caosScript::logicaltokentype caosScript::logicalType(const caostoken* const t) {
	return logicalType(*t);
}

caosScript::logicaltokentype caosScript::logicalType(const caostoken& t) {
	switch (t.type) {
		case caostoken::TOK_WORD:
			return TOK_WORD;
		case caostoken::TOK_BYTESTR:
			if (d->name == "c1" || d->name == "c2") {
				return TOK_CONST;
			} else {
				return TOK_BYTESTR;
			}
		case caostoken::TOK_STRING:
		case caostoken::TOK_CHAR:
		case caostoken::TOK_BINARY:
		case caostoken::TOK_INT:
		case caostoken::TOK_FLOAT:
			return TOK_CONST;
		case caostoken::TOK_EOI:
			return EOI;
		case caostoken::TOK_ERROR:
			throw parseException("no logical type for a lexer error token", t.lineno);
		case caostoken::TOK_COMMENT:
			throw parseException("no logical type for a comment token", t.lineno);
		case caostoken::TOK_WHITESPACE:
			throw parseException("no logical type for a whitespace token", t.lineno);
		case caostoken::TOK_NEWLINE:
			throw parseException("no logical type for a newline token", t.lineno);
		case caostoken::TOK_COMMA:
			throw parseException("no logical type for a comma token", t.lineno);
	}
}

caostoken* caosScript::getToken(logicaltokentype expected) {
	caostoken* t = tokenPeek();
	caostoken dummy;
	caostoken& r = (t ? *t : dummy);
	errindex = curindex;

	if (expected != ANYTOKEN && logicalType(r) != expected) {
		unexpectedToken(r);
	}

	curindex++;

	return t;
}

void caosScript::putBackToken(caostoken*) {
	curindex--;
	errindex = curindex - 1; // curindex refers to the /next/ token to be parsed
		// so make sure we refer to the token before it
}

void caosScript::parse(std::istream& in) {
	// slurp our input stream
	return parse(readfile(in));
}

void caosScript::parse(const std::string& caostext) {
	assert(!tokens);
	// run the token parser
	{
		std::vector<caostoken> rawtokens;
		lexcaos(rawtokens, caostext.c_str());

		tokens = std::shared_ptr<std::vector<caostoken> >(new std::vector<caostoken>());
		size_t index = 0;
		for (auto& t : rawtokens) {
			switch (t.type) {
				case caostoken::TOK_WORD:
					std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
					t.index = index++;
					tokens->push_back(t);
					break;
				case caostoken::TOK_BYTESTR:
				case caostoken::TOK_STRING:
				case caostoken::TOK_CHAR:
				case caostoken::TOK_BINARY:
				case caostoken::TOK_INT:
				case caostoken::TOK_FLOAT:
				case caostoken::TOK_EOI:
				case caostoken::TOK_ERROR:
					t.index = index++;
					tokens->push_back(t);
					break;
				case caostoken::TOK_COMMENT:
				case caostoken::TOK_WHITESPACE:
				case caostoken::TOK_NEWLINE:
				case caostoken::TOK_COMMA:
					break;
			}
		}
	}
	curindex = errindex = traceindex = 0;

	try {
		parseloop(ST_INSTALLER, NULL);

		std::string buf;
		std::shared_ptr<std::vector<toktrace> > tokinfo(new std::vector<toktrace>());
		for (size_t p = 0; p < tokens->size(); p++) {
			std::string tok = (*tokens)[p].format();
			int len = tok.size();
			if (len > 65535) {
				errindex = p;
				throw parseException("Overlong token");
			}
			buf += tok;
			buf += " ";
			tokinfo->push_back(toktrace(len, (*tokens)[p].lineno));
		}
		shared_str code(buf);
		installer->code = code;
		installer->tokinfo = tokinfo;
		installer->link();
		if (removal) {
			removal->link();
			removal->tokinfo = tokinfo;
			removal->code = code;
		}
		std::vector<std::shared_ptr<script> >::iterator i = scripts.begin();
		while (i != scripts.end()) {
			(*i)->tokinfo = tokinfo;
			(*i)->code = code;
			(*i++)->link();
		}
	} catch (parseException& e) {
		e.filename = filename;
		if (!tokens)
			throw;
		if (errindex < 0 || (size_t)errindex >= tokens->size())
			throw;
		e.lineno = (*tokens)[errindex].lineno;
		e.context = std::shared_ptr<std::vector<caostoken> >(new std::vector<caostoken>());
		/* We'd like to capture N tokens on each side of the target, but
		 * if we can't get all those from one side, get it from the other.
		 */
		int contextlen = 5;
		int leftct = contextlen;
		int rightct = contextlen;
		int prefix = 0;

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
			e.context->push_back(caostoken());
			e.context->back().setWord("...");
			prefix = 1;
		}

		for (int i = errindex - leftct; i < errindex + rightct; i++) {
			e.context->push_back((*tokens)[i]);
		}
		if (errindex + rightct + 1 < (int)tokens->size()) {
			e.context->push_back(caostoken());
			e.context->back().setWord("...");
		}
		e.ctxoffset = leftct + prefix;
		throw;
	}
}

caosValue caosScript::asConst(const caostoken& token) {
	switch (token.type) {
		case caostoken::TOK_STRING:
			return caosValue(token.stringval());
		case caostoken::TOK_CHAR:
		case caostoken::TOK_BINARY:
		case caostoken::TOK_INT:
			return caosValue(token.intval());
		case caostoken::TOK_FLOAT:
			return caosValue(token.floatval());
		case caostoken::TOK_BYTESTR: {
			if (d->name == "c1" || d->name == "c2") {
				return caosValue(token.stringval());
			}
			unexpectedToken(token);
		}
		case caostoken::TOK_WORD:
		case caostoken::TOK_COMMENT:
		case caostoken::TOK_WHITESPACE:
		case caostoken::TOK_NEWLINE:
		case caostoken::TOK_COMMA:
		case caostoken::TOK_EOI:
		case caostoken::TOK_ERROR:
			unexpectedToken(token);
	}
}

void caosScript::unexpectedToken(const caostoken& token) {
	throw parseException("Unexpected " + token.typeAsString());
}

const cmdinfo* caosScript::readCommand(caostoken* t, const std::string& prefix, bool except) {
	if (!except && logicalType(t) != TOK_WORD)
		return NULL;

	std::string fullname = prefix + t->word();
	errindex = t->index;
	const cmdinfo* ci = d->find_command(fullname.c_str());

	if (!ci) {
		if (!except)
			return NULL;
		throw parseException(std::string("Command not found: ") + fullname);
	}

	// See if there's a subcommand
	caostoken* t2 = NULL;
	const cmdinfo* subci = NULL;
	bool need_subcmd = (ci->argtypes && ci->argtypes[0] == CI_SUBCOMMAND);

	t2 = getToken(ANYTOKEN);
	if (t2)
		subci = readCommand(t2, fullname + " ", except && need_subcmd);

	if (subci)
		return subci;

	// speculative readahead failed, toss back what we have now
	if (t2)
		putBackToken(t2);

	if (need_subcmd) {
		assert(!except); // we should've exceptioned out already if we were going to
		return NULL;
	}

	return ci;
}

void caosScript::emitOp(opcode_t op, int argument) {
	if (op == CAOS_YIELD && engine.version < 3 && enumdepth > 0)
		return;
	current->ops.push_back(caosOp(op, argument, traceindex));
}

void caosScript::emitExpr(std::shared_ptr<CAOSExpression> ce) {
	ce->eval(this, false);
}

std::shared_ptr<CAOSExpression> caosScript::readExpr(const enum ci_type xtype) {
	caostoken* t = getToken();
	traceindex = errindex = curindex;
	if (xtype == CI_BAREWORD) {
		if (logicalType(t) == TOK_WORD) {
			return std::shared_ptr<CAOSExpression>(new CAOSExpression(errindex, caosValue(t->word())));
		} else if (logicalType(t) == TOK_CONST) {
			if (asConst(*t).getType() != CAOSSTR)
				unexpectedToken(*t);
			return std::shared_ptr<CAOSExpression>(new CAOSExpression(errindex, asConst(*t)));
		} else {
			unexpectedToken(*t);
		}
		assert(!"UNREACHABLE");
		return std::shared_ptr<CAOSExpression>();
	}
	switch (logicalType(t)) {
		case TOK_CONST:
			return std::shared_ptr<CAOSExpression>(new CAOSExpression(errindex, asConst(*t)));
		case TOK_BYTESTR:
			return std::shared_ptr<CAOSExpression>(new CAOSExpression(errindex, t->bytestr()));
		case TOK_WORD: break; // fall through to remainder of function
		default: unexpectedToken(*t);
	}

	std::string oldpayload = t->word();
	std::shared_ptr<CAOSExpression> ce(new CAOSExpression(errindex, CAOSCmd()));
	CAOSCmd* cmd = ce->value.get_if<CAOSCmd>();

	if (t->word().size() == 4 && isdigit(t->word()[2]) && isdigit(t->word()[3])) {
		if (!strncmp(t->word().c_str(), "va", 2) || !strncmp(t->word().c_str(), "ov", 2) || !strncmp(t->word().c_str(), "mv", 2)) {
			int idx = atoi(t->word().c_str() + 2);
			if (!strncmp(t->word().c_str(), "va", 2))
				idx = current->mapVAxx(idx);
			t->setWord(t->word().substr(0, 2) + "xx");
			const cmdinfo* op = readCommand(t, std::string("expr "));
			t->setWord(oldpayload);

			std::shared_ptr<CAOSExpression> arg(new CAOSExpression(errindex, caosValue(idx)));
			cmd->op = op;
			cmd->arguments.push_back(arg);
			return ce;
		}
	}

	if (t->word().size() == 4 && isdigit(t->word()[3]) && engine.version < 3) {
		// OBVx VARx hacks
		if (!strncmp(t->word().c_str(), "obv", 3) || !strncmp(t->word().c_str(), "var", 3)) {
			int idx = atoi(t->word().c_str() + 3);
			if (!strncmp(t->word().c_str(), "var", 3))
				idx = current->mapVAxx(idx);
			t->setWord(t->word().substr(0, 3) + "x");
			const cmdinfo* op = readCommand(t, std::string("expr "));
			t->setWord(oldpayload);

			std::shared_ptr<CAOSExpression> arg(new CAOSExpression(errindex, caosValue(idx)));
			cmd->op = op;
			cmd->arguments.push_back(arg);
			return ce;
		}
	}

	const cmdinfo* ci = readCommand(t, std::string(xtype == CI_COMMAND ? "cmd " : "expr "));
	t->setWord(oldpayload);
	cmd->op = ci;
	for (int i = 0; i < ci->argc; i++) {
		cmd->arguments.push_back(readExpr(ci->argtypes[i]));
	}
	return ce;
}

int caosScript::readCond() {
	caostoken* t = getToken(TOK_WORD);
	typedef struct {
		const char* n;
		int cnd;
	} cond_entry;
	const static cond_entry conds[] = {
		{"eq", CEQ},
		{"=", CEQ},
		{"gt", CGT},
		{">", CGT},
		{"ge", CGE},
		{">=", CGE},
		{"lt", CLT},
		{"<", CLT},
		{"le", CLE},
		{"<=", CLE},
		{"ne", CNE},
		{"<>", CNE},
		{"bt", CBT},
		{"bf", CBF},
		{NULL, 0}};

	const cond_entry* c = conds;
	while (c->n != NULL) {
		if (t->word() == c->n)
			return c->cnd;
		c++;
	}
	throw parseException(std::string("Unexpected non-condition word: ") + t->word());
}

void caosScript::parseCondition() {
	emitOp(CAOS_CONSTINT, 1);

	bool nextIsAnd = true;
	while (1) {
		std::shared_ptr<CAOSExpression> a1, a2;
		a1 = readExpr(CI_ANYVALUE);
		int cond = readCond();
		a2 = readExpr(CI_ANYVALUE);
		emitExpr(a1);
		emitExpr(a2);
		emitOp(CAOS_COND, cond | (nextIsAnd ? CAND : COR));

		caostoken* peek = tokenPeek();
		if (!peek)
			break;
		if (logicalType(peek) != TOK_WORD)
			break;
		if (peek->word() == "and") {
			getToken();
			nextIsAnd = true;
		} else if (peek->word() == "or") {
			getToken();
			nextIsAnd = false;
		} else
			break;
	}
}

void caosScript::parseloop(int state, void* info) {
	caostoken* t;
	while ((t = getToken(ANYTOKEN))) {
		traceindex = errindex;
		if (logicalType(t) == EOI) {
			switch (state) {
				case ST_INSTALLER:
				case ST_BODY:
				case ST_REMOVAL:
					return;
				default:
					throw parseException("Unexpected end of input");
			}
		}
		if (logicalType(t) != TOK_WORD) {
			throw parseException("Unexpected non-word token");
		}
		if (t->word() == "scrp") {
			if (state != ST_INSTALLER && state != ST_BODY && state != ST_REMOVAL)
				throw parseException("Unexpected SCRP");
			assert(!enumdepth);
			state = ST_BODY;
			int bits[4];
			for (int& bit : bits) {
				caosValue val = asConst(*getToken(TOK_CONST));
				if (val.getType() != CAOSINT)
					throw parseException("Expected integer constant");
				bit = val.getInt();
			}
			int fmly = bits[0];
			int gnus = bits[1];
			int spcs = bits[2];
			int scrp = bits[3];
			scripts.push_back(std::shared_ptr<script>(new script(d, filename, fmly, gnus, spcs, scrp)));
			current = scripts.back();
		} else if (t->word() == "rscr") {
			if (state == ST_INSTALLER || state == ST_BODY || state == ST_REMOVAL)
				state = ST_REMOVAL;
			else
				throw parseException("Unexpected RSCR");
			if (!removal)
				removal = std::shared_ptr<script>(new script(d, filename));
			current = removal;
		} else if (t->word() == "iscr") {
			if (state == ST_INSTALLER || state == ST_BODY || state == ST_REMOVAL)
				state = ST_INSTALLER;
			else
				throw parseException("Unexpected RSCR");
			assert(!enumdepth);
			current = installer;
		} else if (t->word() == "endm") {
			emitOp(CAOS_STOP, 0);

			if (state == ST_INSTALLER || state == ST_BODY || state == ST_REMOVAL) {
				assert(!enumdepth);
				state = ST_INSTALLER;
				current = installer;
			} else if (state == ST_ENUM || state == ST_ESCN) {
				// fuzzie added this case because she can't remember why the DIAF is here,
				// if you work it out please add a comment and make it engine-specific!
				throw parseException("Unexpected ENDM");
			} else if (state == ST_DOIF) {
				// this case too
				throw parseException("Unexpected ENDM");
			} else {
				// I hate you. Die in a fire.
				putBackToken(t);
				return;
			}
			// No we will not emit c_ENDM() thankyouverymuch

		} else if (t->word() == "enum" || t->word() == "esee" || t->word() == "etch" || t->word() == "epas" || t->word() == "econ") {
			int nextreloc = current->newRelocation();
			putBackToken(t);

			enumdepth++;
			emitExpr(readExpr(CI_COMMAND));
			emitOp(CAOS_YIELD, 0);
			emitOp(CAOS_JMP, nextreloc);
			int startp = current->getNextIndex();
			parseloop(ST_ENUM, NULL);
			enumdepth--;
			current->fixRelocation(nextreloc);

			emitCmd("cmd next");
			emitOp(CAOS_ENUMPOP, startp);
		} else if (t->word() == "next") {
			if (state != ST_ENUM) {
				throw parseException("Unexpected NEXT");
			}
			return;
		} else if (t->word() == "escn") {
			int nextreloc = current->newRelocation();
			putBackToken(t);

			enumdepth++;
			emitExpr(readExpr(CI_COMMAND));
			emitOp(CAOS_YIELD, 0);
			emitOp(CAOS_JMP, nextreloc);
			int startp = current->getNextIndex();
			parseloop(ST_ESCN, NULL);
			enumdepth--;
			current->fixRelocation(nextreloc);

			emitCmd("cmd nscn");
			emitOp(CAOS_ENUMPOP, startp);
		} else if (t->word() == "nscn") {
			if (state != ST_ESCN) {
				throw parseException("Unexpected NSCN");
			}
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
			emitCmd("cmd loop");
			parseloop(ST_LOOP, (void*)&loop);
		} else if (t->word() == "untl") {
			if (state != ST_LOOP)
				throw parseException("Unexpected UNTL");
			// TODO: zerocost logic inversion - do in c_UNTL()?
			int loop = *(int*)info;
			int out = current->newRelocation();
			parseCondition();
			emitCmd("cmd untl");
			emitOp(CAOS_CJMP, out);
			emitOp(CAOS_JMP, loop);
			current->fixRelocation(out);
			return;
		} else if (t->word() == "ever") {
			if (state != ST_LOOP)
				throw parseException("Unexpected EVER");
			int loop = *(int*)info;
			emitOp(CAOS_JMP, loop);
			return;

		} else if (t->word() == "reps") {
			struct repsinfo ri;
			ri.jnzreloc = current->newRelocation();
			putBackToken(t);
			emitExpr(readExpr(CI_COMMAND));
			emitOp(CAOS_YIELD, 0);
			emitOp(CAOS_JMP, ri.jnzreloc);
			ri.loopidx = current->getNextIndex();
			parseloop(ST_REPS, (void*)&ri);
		} else if (t->word() == "repe") {
			if (state != ST_REPS)
				throw parseException("Unexpected repe");
			struct repsinfo* ri = (repsinfo*)info;
			current->fixRelocation(ri->jnzreloc);
			emitOp(CAOS_DECJNZ, ri->loopidx);
			emitCmd("cmd repe");
			return;

		} else if (t->word() == "doif") {
			struct doifinfo di;
			di.donereloc = current->newRelocation();
			di.failreloc = current->newRelocation();
			int okreloc = current->newRelocation();

			parseCondition();
			emitCmd("cmd doif");
			emitOp(CAOS_CJMP, okreloc);
			emitOp(CAOS_JMP, di.failreloc);
			current->fixRelocation(okreloc);
			parseloop(ST_DOIF, (void*)&di);
			if (di.failreloc)
				current->fixRelocation(di.failreloc);
			current->fixRelocation(di.donereloc);
			emitCmd("cmd endi");
		} else if (t->word() == "elif") {
			if (state != ST_DOIF) {
				// XXX this is horrible
				t->setWord(std::string("doif"));
				continue;
			}
			struct doifinfo* di = (struct doifinfo*)info;
			int okreloc = current->newRelocation();

			emitOp(CAOS_JMP, di->donereloc);
			current->fixRelocation(di->failreloc);
			di->failreloc = current->newRelocation();
			parseCondition();
			emitCmd("cmd elif");
			emitOp(CAOS_CJMP, okreloc);
			emitOp(CAOS_JMP, di->failreloc);
			current->fixRelocation(okreloc);
			parseloop(ST_DOIF, info);
			return;
		} else if (t->word() == "else") {
			if (state != ST_DOIF)
				throw parseException("Unexpected ELSE");
			struct doifinfo* di = (struct doifinfo*)info;
			if (!di->failreloc)
				throw parseException("Duplicate ELSE");
			emitOp(CAOS_JMP, di->donereloc);
			current->fixRelocation(di->failreloc);
			di->failreloc = 0;
			emitCmd("cmd else");
		} else if (t->word() == "endi") {
			if (state != ST_DOIF) {
				if (engine.version >= 3)
					throw parseException("Unexpected ENDI");
				// you are a horrible person if you get here
				// damn you CL coders.
				continue;
			}
			return;
		} else if (t->word() == "ssfc") {
			std::shared_ptr<CAOSExpression> roomno_e = readExpr(CI_NUMERIC);

			caosValue coordcount = asConst(*getToken(TOK_CONST));
			if (!coordcount.hasInt())
				throw parseException("Literal integer expected");
			int count = coordcount.getInt();

			std::vector<std::pair<int, int> > points(count);
			for (int i = 0; i < count; i++) {
				caosValue cvx = asConst(*getToken(TOK_CONST));
				if (!cvx.hasInt())
					throw parseException("Literal integer expected");
				caosValue cvy = asConst(*getToken(TOK_CONST));
				if (!cvy.hasInt())
					throw parseException("Literal integer expected");
				points[i].first = cvx.getInt();
				points[i].second = cvy.getInt();
			}

			// emit the values in backwards order
			for (int i = points.size() - 1; i >= 0; i--) {
				// y first
				emitConst(caosValue(points[i].second));
				emitConst(caosValue(points[i].first));
			}
			emitConst(coordcount);
			emitExpr(roomno_e);
			emitCmd("cmd ssfc");
		} else {
			if (t->word() == "dbg:") {
				caostoken* t2 = tokenPeek();
				if (t2 && logicalType(t2) == TOK_WORD && t2->word() == "asrt") {
					getToken(TOK_WORD);
					emitOp(CAOS_CONSTINT, 1);
					parseCondition();
					int endreloc = current->newRelocation();
					emitOp(CAOS_CJMP, endreloc);
					emitCmd("cmd dbg: asrt");
					current->fixRelocation(endreloc);
					continue;
				}
				if (t2 && logicalType(t2) == TOK_WORD && t2->word() == "asrf") {
					getToken(TOK_WORD);
					emitOp(CAOS_CONSTINT, 1);
					parseCondition();
					int okreloc = current->newRelocation();
					int failreloc = current->newRelocation();
					emitOp(CAOS_CJMP, okreloc);
					emitOp(CAOS_JMP, failreloc);
					current->fixRelocation(okreloc);
					emitCmd("cmd dbg: asrf");
					current->fixRelocation(failreloc);
					continue;
				}
			}
			putBackToken(t);
			emitExpr(readExpr(CI_COMMAND));
			emitOp(CAOS_YIELD, 0);
		}
	}
}

void caosScript::emitCmd(const char* name) {
	const cmdinfo* ci = d->find_command(name);
	emitOp(CAOS_CMD, d->cmd_index(ci));
	emitOp(CAOS_YIELD, 0);
}

void CAOSExpression::eval(caosScript* scr, bool save_here) const {
	if (auto* cmd_p = value.get_if<CAOSCmd>()) {
		const auto& cmd = *cmd_p;
		for (size_t i = 0; i < cmd.arguments.size(); i++) {
			bool save_there = (i < (size_t)cmd.op->argc && cmd.op->argtypes[i] == CI_VARIABLE);
			cmd.arguments[i]->eval(scr, save_there);
		}
		scr->traceindex = cmd.traceidx - 1;
		// If we're to be invoked to save our result later,
		// stash our args for that time.
		if (save_here) {
			// Note: These indices refer to stack positions, with 0 being the top.
			// We thus transfer the arguments in reverse order, as the order will again be
			// reversed when the stack is restored.
			for (size_t i = 0; i < cmd.arguments.size(); i++)
				scr->emitOp(CAOS_PUSH_AUX, i);
		}
		scr->emitOp(CAOS_CMD, scr->d->cmd_index(cmd.op));
		// If we emit variable-result arguments as well, we need to move our
		// result down below them.
		// This is theoretical at the moment - no expression-type commands also
		// write back to their args.

		if (cmd.op->rettype != CI_COMMAND) {
			int rotcount = 0;
			for (int i = 0; i < cmd.op->argc; i++) {
				if (cmd.op->argtypes[i] == CI_VARIABLE)
					rotcount++;
			}
			if (rotcount)
				scr->emitOp(CAOS_STACK_ROT, rotcount);
		}
		for (int i = cmd.arguments.size() - 1; i >= 0; i--) {
			if (i < cmd.op->argc && cmd.op->argtypes[i] == CI_VARIABLE)
				cmd.arguments[i]->save(scr);
		}

	} else if (auto* v = value.get_if<caosValue>()) {
		scr->emitConst(*v);
	} else {
		std::terminate();
	}
}

void CAOSExpression::save(caosScript* scr) const {
	if (auto* cmd = value.get_if<CAOSCmd>()) {
		scr->errindex = scr->traceindex = cmd->traceidx - 1;
		if (cmd->op->rettype != CI_VARIABLE) {
			throw parseException(std::string("RValue ") + cmd->op->fullname + " used where LValue expected");
		}
		scr->emitOp(CAOS_RESTORE_AUX, cmd->arguments.size());
		scr->emitOp(CAOS_SAVE_CMD, scr->d->cmd_index(cmd->op));

	} else if (auto* v = value.get_if<caosValue>()) {
		(void)v;
	} else {
		std::terminate();
	}
}


int script::mapVAxx(int index) {
	assert(index >= 0 && index < 100);
	if (varRemap[index] != 0xFF) {
		assert(varRemap[index] < 100);
		return varRemap[index];
	}
	return (varRemap[index] = varUsed++);
}

/* vim: set noet: */
