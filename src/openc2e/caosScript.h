/*
 *  caosScript.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#ifndef CAOSSCRIPT_H
#define CAOSSCRIPT_H

#include "bytecode.h"
#include "caosValue.h"
#include "cmddata.h"
#include "common/SimpleVariant.h"
#include "common/shared_str.h"
#include "dialect.h"
#include "fileformats/caostoken.h"
#include "serfwd.h"

#include <cassert>
#include <fmt/core.h>
#include <istream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>


class Agent;

struct toktrace {
	unsigned short width;
	unsigned short lineno;

	toktrace(unsigned short w, unsigned short l)
		: width(w), lineno(l) {}

  private:
	FRIEND_SERIALIZE(toktrace)
	toktrace() {}
};

class script {
  protected:
	FRIEND_SERIALIZE(script)

	bool linked;

	// position 0 is reserved in the below vector
	// relocations[-relocid] is the target address for relocation relocid
	// will be 0 if unresolved
	std::vector<int> relocations;
	// map of name -> (address|relocation)
	std::map<std::string, int> labels;
	script() {
		memset(varRemap, 0xFF, 100);
		varUsed = 0;
		linked = false;
	}
	// remapping array for VAxx
	unsigned char varRemap[100], varUsed;

  public:
	// ops[0] is initted to a nop, as address 0 is reserved for a flag value
	// in the relocation vector
	std::vector<caosOp> ops;
	// table of all non-trivial constants in the script
	// small immediates integers are done with CAOS_CONSTINT
	// mostly for strings and floats
	std::vector<caosValue> consts;
	// a normalized copy of the script source. this is used for error tracing
	shared_str code;
	std::shared_ptr<std::vector<toktrace> > tokinfo;

  public:
	int fmly, gnus, spcs, scrp;
	const class Dialect* dialect;
	const Dialect* getDialect() const { return dialect; };

	std::string filename;

	caosOp getOp(int idx) const {
		assert(idx >= 0);
		return (size_t)idx >= ops.size() ? caosOp(CAOS_DIE, -1, -1) : ops[idx];
	}

	int scriptLength() const {
		return ops.size();
	}

	caosValue getConstant(int idx) const {
		if (idx < 0 || (size_t)idx >= consts.size()) {
			throw caosException(
				fmt::format("Internal error: const {} out of range", idx));
		}
		return consts[idx];
	}

	std::map<std::string, int> gsub;
	int getNextIndex() { return ops.size(); }
	// add op as the next opcode
	script(const Dialect* v, const std::string& fn,
		int fmly_, int gnus_, int spcs_, int scrp_);
	script(const Dialect* v, const std::string& fn);
	~script();
	std::string dump();
	//	std::string dumpLine(unsigned int);

	void link();

	int newRelocation() {
		assert(!linked);
		int idx = relocations.size();
		relocations.push_back(0);
		return -idx;
	}

	void fixRelocation(int r, int p) {
		assert(!linked);
		assert(r < 0);
		r = -r;
		assert(relocations[r] == 0);
		// check for a loop
		int i = p;
		while (i < 0) {
			i = -i;
			if (i == r)
				throw Exception("relocation loop found");
			i = relocations[i];
		}

		relocations[r] = p;
	}

	// fix relocation r to point to the next op to be emitted
	void fixRelocation(int r) {
		fixRelocation(r, getNextIndex());
	}

	// find the address of the given label, could be a relocation
	int getLabel(const std::string& label) {
		if (labels.find(label) == labels.end())
			labels[label] = newRelocation();
		return labels[label];
	}

	// fix a label to the end of the current op string
	void affixLabel(const std::string& label) {
		int reloc = getLabel(label);
		if (reloc > 0)
			throw caosException(std::string("Label ") + label + " redefined");
		fixRelocation(reloc);
		labels[label] = getNextIndex();
	}

	void emitOp(opcode_t op, int argument);

	int mapVAxx(int index);
	int varsNeeded() const { return varUsed; }
};

// parser tree

class caosScript;

struct CAOSExpression;

struct CAOSCmd {
	const cmdinfo* op;
	std::vector<std::shared_ptr<CAOSExpression> > arguments;
	int traceidx;
	CAOSCmd() {
		op = 0;
		traceidx = -1;
	}
	CAOSCmd(const CAOSCmd& c)
		: op(c.op), arguments(c.arguments) {}
};

struct CAOSExpression {
	SimpleVariant<CAOSCmd, caosValue> value;
	int traceidx;
	void eval(caosScript* scr, bool save_here) const;
	void save(caosScript* scr) const;

	CAOSExpression(const CAOSExpression& e)
		: value(e.value), traceidx(e.traceidx) {}
	CAOSExpression(int idx, const CAOSCmd& c)
		: value(c), traceidx(idx) { value.get<CAOSCmd>().traceidx = traceidx; }
	CAOSExpression(int idx, const caosValue& c)
		: value(c), traceidx(idx) {}
};

class caosScript { //: Collectable {
  public:
	const Dialect* d;
	std::string filename;
	std::shared_ptr<script> installer, removal;
	std::vector<std::shared_ptr<script> > scripts;
	std::shared_ptr<script> current;

	caosScript(const std::string& dialect, const std::string& filename);
	caosScript() { d = NULL; }
	void parse(const std::string& caostext);
	void parse(std::istream& in);
	~caosScript();
	void installScripts();
	void installInstallScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short eventid);

  protected:
	int readCond();
	void parseCondition();
	void emitOp(opcode_t op, int argument);
	void emitCmd(const char* name);
	void emitConst(const caosValue&);
	std::shared_ptr<CAOSExpression> readExpr(const enum ci_type xtype);
	void emitExpr(std::shared_ptr<CAOSExpression> ce);
	const cmdinfo* readCommand(caostoken* t, const std::string& prefix, bool except = true);
	void parseloop(int state, void* info);

	caosValue asConst(const caostoken& token);
	[[noreturn]] void unexpectedToken(const caostoken& token);

	enum logicaltokentype {
		ANYTOKEN = 0,
		EOI = 0,
		TOK_WORD,
		TOK_BYTESTR,
		TOK_CONST,
	};

	logicaltokentype logicalType(const caostoken* const);
	logicaltokentype logicalType(const caostoken&);

	std::shared_ptr<std::vector<caostoken> > tokens;
	int curindex; // index to the next token to be read
	int errindex; // index to the token to report parse errors on
	int traceindex; // index to the token to report runtime errors on
	int enumdepth;
	// deprecated support functions
	caostoken* tokenPeek();
	void putBackToken(caostoken*);
	caostoken* getToken(logicaltokentype expected = ANYTOKEN);

	friend void CAOSExpression::save(caosScript*) const;
	friend void CAOSExpression::eval(caosScript* scr, bool save_here) const;
};

#endif
/* vim: set noet: */
