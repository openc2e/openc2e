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
#include <cassert>
#include <boost/shared_ptr.hpp>
#include "openc2e.h"

class Agent;
class caosOp;
class Variant;

using boost::shared_ptr;

struct script {
	protected:
		FRIEND_SERIALIZE(script);
		
		bool linked;

		// position 0 is reserved in the below vector
		std::vector<int> relocations;
		// pos-0 needs to be initted to a caosNoop
		std::vector<class caosOp *> allOps;

		script() {}
	public:
		int fmly, gnus, spcs, scrp;
		const Variant *variant;
		const Variant *getVariant() const { return variant; };
		
		std::string filename;

		caosOp *getOp(int idx) const {
			assert (idx >= 0);
			return (unsigned int)idx >= allOps.size() ? NULL : allOps[idx];
		}
		
		std::map<std::string, int> gsub;
		int getNextIndex() { return allOps.size(); }
		// add op as the next opcode
		void thread(caosOp *op);
		script(const Variant *v, const std::string &fn,
				int fmly_, int gnus_, int spcs_, int scrp_);
		script(const Variant *v, const std::string &fn);
		~script();
		std::string dump();
	//	std::string dumpLine(unsigned int);

		void link();
		
		int newRelocation() {
			assert (!linked);
			int idx = relocations.size();
			relocations.push_back(0);
			return -idx;
		}

		// fix relocation r to point to the next op to be emitted
		// XXX: maybe make relocations lightweight classes, so we
		// can identify leaks.

		void fixRelocation(int r, int p) {
			assert (!linked);
			assert (r < 0);
			r = -r;
			assert (relocations[r] == 0);
			// check for a loop
			int i = p;
			while (i < 0) {
				if (i == r)
					throw creaturesException("relocation loop found");
				i = relocations[-i];
			}
			
			relocations[r] = p;
		}
			
		void fixRelocation(int r) {
			fixRelocation(r, getNextIndex());
		}

		
};

class caosScript { //: Collectable {
public:
	const Variant *v;
	std::string filename;
	shared_ptr<script> installer, removal;
	std::vector<shared_ptr<script> > scripts;
	shared_ptr<script> current;

	caosScript(const std::string &variant, const std::string &fn);
	caosScript() { v = NULL; }
	void parse(std::istream &in);
	~caosScript();
	void installScripts();
	void installInstallScript(unsigned char family, unsigned char genus, unsigned short species, unsigned short eventid);
};

#endif
/* vim: set noet: */
