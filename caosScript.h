/*
 *  caosVM.h
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

#include <vector>
#include <list>
#include <string>
#include <istream>
#include <map>
#include "caosVar.h"

class Agent;
class caosOp;

struct script { //: public Collectable {
	std::string filename;
	std::vector<class caosOp *> allOps;
	std::map<std::string, class caosOp *> gsub;
	caosOp *entry, *last;

	// add op as the next opcode
	void thread(caosOp *op);
	void addOp(caosOp *op);
	script(const std::string &fn);
	~script();
//	std::string dump();
//	std::string dumpLine(unsigned int);

	void release(){}
	void retain(){}

};

struct residentScript {
	int fmly, gnus, spcs, scrp;
	script *s;

	~residentScript() {
		s->release();
	}

	residentScript(int f, int g, int s_, int scrp_, script *scr)
		: fmly(f), gnus(g), spcs(s_), scrp(scrp_), s(scr) {
			s->retain();
	}

	residentScript(const residentScript &rs) {
		fmly = rs.fmly; gnus = rs.gnus; spcs = rs.spcs; scrp = rs.scrp;
		s = rs.s;
		s->retain();
	}
};

class caosScript { //: Collectable {
public:
	std::string filename;
	script *installer, *removal;
	std::vector<residentScript> scripts;
	script *current;

	caosScript(const std::string &fn);
	void parse(std::istream &in);
	~caosScript();
	void installScripts();

	void release() {}
	void retain() {}
};

#endif
/* vim: set noet: */
