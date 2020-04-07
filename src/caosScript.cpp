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
#include "caosScript.h"
#include "caoslexer.h"
#include "caosVM.h"
#include "openc2e.h"
#include "Engine.h"
#include "World.h"
#include "token.h"
#include "dialect.h"
#include "util.h"
#include "Scriptorium.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <fmt/printf.h>

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
	memset(varRemap, 0xFF, 100);
	varUsed = 0;
	linked = false;
}
	
script::script(const Dialect *v, const std::string &fn,
		int fmly_, int gnus_, int spcs_, int scrp_)
	: fmly(fmly_), gnus(gnus_), spcs(spcs_), scrp(scrp_),
		dialect(v), filename(fn)
{
	ops.push_back(caosOp(CAOS_NOP, 0, -1));
	relocations.push_back(0);
	memset(varRemap, 0xFF, 100);
	varUsed = 0;
	linked = false;
}

std::string script::dump() {
	std::ostringstream oss;
	oss << "Relocations:" << std::endl;
	for (unsigned int i = 1; i < relocations.size(); i++) {
		oss << fmt::sprintf("%08d -> %08d", i, relocations[i]) << std::endl;
	}
	oss << "Code:" << std::endl;
	for (unsigned int i = 0; i < ops.size(); i++) {
		oss << fmt::sprintf("%08d: ", i);
		oss << dumpOp(dialect, ops[i]);
		oss << std::endl;
	}
	return oss.str();
}

caosScript::caosScript(const std::string &dialect, const std::string &fn) {
	enumdepth = 0;
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


saveVisit::saveVisit(caosScript *s)
	: scr(s)
{ }

void saveVisit::operator()(const CAOSCmd &cmd) const {
	scr->errindex = scr->traceindex = cmd.traceidx - 1;
	if (cmd.op->rettype != CI_VARIABLE) {
		throw parseException(std::string("RValue ") + cmd.op->fullname + " used where LValue expected");
	}
	scr->emitOp(CAOS_RESTORE_AUX, cmd.arguments.size());
	scr->emitOp(CAOS_SAVE_CMD, scr->d->cmd_index(cmd.op));
}

evalVisit::evalVisit(caosScript *s, bool save_here_)
	: scr(s), save_here(save_here_)
{ }



void evalVisit::operator()(const CAOSCmd &cmd) const {
	for (size_t i = 0; i < cmd.arguments.size(); i++) {
		bool save_there = (cmd.op->argtypes[i] == CI_VARIABLE);
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
		for (size_t i = 0; cmd.op->argtypes[i] != CI_END; i++) {
			if (cmd.op->argtypes[i] == CI_VARIABLE)
				rotcount++;		
		}
		if (rotcount)
			scr->emitOp(CAOS_STACK_ROT, rotcount);
	}
	for (int i = cmd.arguments.size() - 1; i >= 0; i--) {
		if (cmd.op->argtypes[i] == CI_VARIABLE)
			cmd.arguments[i]->save(scr);
	}
}

void evalVisit::operator()(const caosVar &v) const {
	scr->emitConst(v);
}

void caosScript::emitConst(const caosVar &v) {
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

void evalVisit::operator()(const bytestring_t &bs) const {
	scr->current->bytestrs.push_back(bs);
	scr->emitOp(CAOS_BYTESTR, scr->current->bytestrs.size() - 1);
}

int costVisit::operator()(const CAOSCmd &cmd) const {
	int accum = cmd.op->evalcost;
	for (size_t i = 0; i < cmd.arguments.size(); i++)
		accum += cmd.arguments[i]->cost();
	return accum;
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

struct repsinfo {
	int jnzreloc;
	int loopidx;
};


token *caosScript::tokenPeek() {
	while (true) {
		if ((size_t)curindex >= tokens->size()) {
			return NULL;
		}
		if ((*tokens)[curindex].type == token::TOK_COMMENT) {
			curindex++;
			continue;
		}
		return &(*tokens)[curindex];
	}
}

caosScript::logicaltokentype caosScript::logicalType(const token * const t ) {
	return logicalType(*t);
}

caosScript::logicaltokentype caosScript::logicalType(const token& t) {
	switch (t.type) {
		case token::TOK_WORD:
			return TOK_WORD;
		case token::TOK_BYTESTR:
			return TOK_BYTESTR;
		case token::TOK_STRING:
		case token::TOK_CHAR:
		case token::TOK_BINARY:
		case token::TOK_INT:
		case token::TOK_FLOAT:
			return TOK_CONST;
		case token::TOK_EOI:
			return EOI;
		case token::TOK_ERROR:
			throw parseException("no logical type for a lexer error token");
		case token::TOK_COMMENT:
			throw parseException("no logical type for a comment token");
	}
}

token *caosScript::getToken(logicaltokentype expected) {
	token *t = tokenPeek();
	while (t && t->type == token::TOK_COMMENT) {
		curindex++;
		t = tokenPeek();
	}

	token dummy;
	token &r = (t ? *t : dummy);
	errindex = curindex;

	if (expected != ANYTOKEN && logicalType(r) != expected) {
		unexpectedToken(r);
	}

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
	// slurp our input stream
	std::string caostext = readfile(in);
	// run the token parser
	{
		bool using_c2;
		using_c2 = (d->name == "c1" || d->name == "c2");

		tokens = shared_ptr<std::vector<token> >(new std::vector<token>());
		lexcaos(*tokens, caostext.c_str(), using_c2);
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
		shared_str code(oss.str());
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
		e.context = std::shared_ptr<std::vector<token> >(new std::vector<token>());
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
			e.context->push_back(token());
			e.context->back().setWord("...");
			prefix = 1;
		}

		for (int i = errindex - leftct; i < errindex + rightct; i++) {
			e.context->push_back((*tokens)[i]);
		}
		if (errindex + rightct + 1 < (int)tokens->size()) {
			e.context->push_back(token());
			e.context->back().setWord("...");
		}
		e.ctxoffset = leftct + prefix;
		throw;
	}
}

caosVar caosScript::asConst(const token& token) {
	if (token.type == token::TOK_STRING) {
		return caosVar(token.strval);
	}
	if (token.type == token::TOK_CHAR || token.type == token::TOK_BINARY
		|| token.type == token::TOK_INT)
	{
		return caosVar(token.intval);
	}
	if (token.type == token::TOK_FLOAT)
	{
		return caosVar(token.floatval);
	}
	unexpectedToken(token);
}

void caosScript::unexpectedToken(const token& token) {
	throw parseException("Unexpected " + token.typeAsString());
}

const cmdinfo *caosScript::readCommand(token *t, const std::string &prefix, bool except) {
	if (!except && logicalType(t) != TOK_WORD)
		return NULL;

	std::string fullname = prefix + t->word();
	errindex = t->index;
	const cmdinfo *ci = d->find_command(fullname.c_str());

	if (!ci) {
		if (!except)
			return NULL;
		throw parseException(std::string("Command not found: ") + fullname);
	}

	// See if there's a subcommand
	token *t2 = NULL;
	const cmdinfo *subci = NULL;
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
	int cost = ce->cost();
	if (cost)
		emitOp(CAOS_YIELD, cost);
}

std::shared_ptr<CAOSExpression> caosScript::readExpr(const enum ci_type xtype) {
	token *t = getToken();
	traceindex = errindex = curindex;
	if (xtype == CI_BAREWORD) {
		if (logicalType(t) == TOK_WORD) {
			return std::shared_ptr<CAOSExpression>(new CAOSExpression(errindex, caosVar(t->word())));
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
	if (t->word() == "face" && xtype != CI_COMMAND) {
		// horrible hack, yay
		if (xtype == CI_NUMERIC)
			t->setWord("face int");
		else
			t->setWord("face string");
	}

	std::shared_ptr<CAOSExpression> ce(new CAOSExpression(errindex, CAOSCmd()));
	CAOSCmd *cmd = mpark::get_if<CAOSCmd>(&ce->value);

	if (t->word().size() == 4 && isdigit(t->word()[2]) && isdigit(t->word()[3])) {
		if (	!strncmp(t->word().c_str(), "va", 2)
			||	!strncmp(t->word().c_str(), "ov", 2)
			||	!strncmp(t->word().c_str(), "mv", 2)) {
			int idx = atoi(t->word().c_str() + 2);
			if (!strncmp(t->word().c_str(), "va", 2))
				idx = current->mapVAxx(idx);
			t->setWord(t->word().substr(0, 2) + "xx");
			const cmdinfo *op = readCommand(t, std::string("expr "));
			t->setWord(oldpayload);

			std::shared_ptr<CAOSExpression> arg(new CAOSExpression(errindex, caosVar(idx)));
			cmd->op = op;
			cmd->arguments.push_back(arg);
			return ce;
		}
	}

	if (t->word().size() == 4 && isdigit(t->word()[3]) && engine.version < 3) {
		// OBVx VARx hacks
		if (	!strncmp(t->word().c_str(), "obv", 3)
			||	!strncmp(t->word().c_str(), "var", 3)) {
			int idx = atoi(t->word().c_str() + 3);
			if (!strncmp(t->word().c_str(), "var", 3))
				idx = current->mapVAxx(idx);
			t->setWord(t->word().substr(0, 3) + "x");
			const cmdinfo *op = readCommand(t, std::string("expr "));
			t->setWord(oldpayload);

			std::shared_ptr<CAOSExpression> arg(new CAOSExpression(errindex, caosVar(idx)));
			cmd->op = op;
			cmd->arguments.push_back(arg);
			return ce;
		}
	}
	
	const cmdinfo *ci = readCommand(t, std::string(xtype == CI_COMMAND ? "cmd " : "expr "));
	t->setWord(oldpayload);
	cmd->op = ci;
	for (int i = 0; ci->argtypes[i] != CI_END; i++) {
		cmd->arguments.push_back(readExpr(ci->argtypes[i]));
	}
	return ce;
}

int caosScript::readCond() {
	token *t = getToken(TOK_WORD);
	typedef struct { const char *n; int cnd; } cond_entry;
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

		token *peek = tokenPeek();
		if (!peek) break;
		if (logicalType(peek) != TOK_WORD) break;
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
			for (int i = 0; i < 4; i++) {
				caosVar val = asConst(*getToken(TOK_CONST));
				if (val.getType() != CAOSINT)
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
			assert(!enumdepth);
			current = installer;
		} else if (t->word() == "endm") {
			emitOp(CAOS_STOP, 0);
			
			if (state == ST_INSTALLER || state == ST_BODY || state == ST_REMOVAL) {
				assert(!enumdepth);
				state = ST_INSTALLER;
				current = installer;
			} else if (state == ST_ENUM) {
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

		} else if (t->word() == "enum"
				|| t->word() == "esee"
				|| t->word() == "etch"
				|| t->word() == "epas"
				|| t->word() == "econ") {
			int nextreloc = current->newRelocation();
			putBackToken(t);

			enumdepth++;
			emitExpr(readExpr(CI_COMMAND));
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
			parseloop(ST_LOOP, (void *)&loop);			
		} else if (t->word() == "untl") {
			if (state != ST_LOOP)
				throw parseException("Unexpected UNTL");
			// TODO: zerocost logic inversion - do in c_UNTL()?
			int loop = *(int *)info;
			int out  = current->newRelocation();
			parseCondition();
			emitCmd("cmd untl");
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
			struct repsinfo ri;
			ri.jnzreloc = current->newRelocation();
			putBackToken(t);
			emitExpr(readExpr(CI_COMMAND));
			emitOp(CAOS_JMP, ri.jnzreloc);
			ri.loopidx = current->getNextIndex();
			parseloop(ST_REPS, (void *)&ri);
		} else if (t->word() == "repe") {
			if (state != ST_REPS)
				throw parseException("Unexpected repe");
			struct repsinfo *ri = (repsinfo *)info;
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
			parseloop(ST_DOIF, (void *)&di);
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
			struct doifinfo *di = (struct doifinfo *)info;
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
			struct doifinfo *di = (struct doifinfo *)info;
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

			caosVar coordcount = asConst(*getToken(TOK_CONST));
			if (!coordcount.hasInt())
				throw parseException("Literal integer expected");
			int count = coordcount.getInt();

			std::vector<std::pair<int, int> > points(count);
			for (int i = 0; i < count; i++) {
				caosVar cvx = asConst(*getToken(TOK_CONST));
				if (!cvx.hasInt())
					throw parseException("Literal integer expected");
				caosVar cvy = asConst(*getToken(TOK_CONST));
				if (!cvy.hasInt())
					throw parseException("Literal integer expected");
				points[i].first = cvx.getInt();
				points[i].second = cvy.getInt();
			}

			// emit the values in backwards order
			for (int i = points.size() - 1; i >= 0; i--) {
				// y first
				emitConst(caosVar(points[i].second));
				emitConst(caosVar(points[i].first));
			}
			emitConst(coordcount);
			emitExpr(roomno_e);
			emitCmd("cmd ssfc");
		} else {
			if (t->word() == "dbg:") {
				token *t2 = tokenPeek();
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
			}
			putBackToken(t);
			emitExpr(readExpr(CI_COMMAND));
		}
	}
}
			
void caosScript::emitCmd(const char *name) {
	const cmdinfo *ci = d->find_command(name);
	emitOp(CAOS_CMD, d->cmd_index(ci));
	if (ci->evalcost)
		emitOp(CAOS_YIELD, ci->evalcost);
}

void CAOSExpression::eval(caosScript *scr, bool save_here) const {
	mpark::visit(evalVisit(scr, save_here), value);
}

void CAOSExpression::save(caosScript *scr) const {
	mpark::visit(saveVisit(scr), value);
}

int CAOSExpression::cost() const {
	return mpark::visit(costVisit(), value);
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
