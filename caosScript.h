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
//#include "bytecode.h"
#include "serialization.h"
#include "dialect.h"

class Agent;
class caosOp;
class Variant;

using boost::shared_ptr;

struct script {
	private:
	friend class boost::serialization::access;
	template<class Archive>
		void load(Archive &ar, unsigned long version) {
			ar & linked & relocations & allOps &
				fmly & gnus & spcs & scrp;
			std::string vname;
			ar & vname;
			if (variants.find(vname) == variants.end())
				abort(); // XXX
			variant = variants[vname];
			ar & filename & gsub;
		}
	template<class Archive>
		void save(Archive &ar, unsigned long version) const {
			ar & linked & relocations & allOps &
				fmly & gnus & spcs & scrp;
			ar & variant->name;
			ar & filename & gsub;
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()
			
	protected:
		
		bool linked;

		// position 0 is reserved in the below vector
		std::vector<int> relocations;
		// pos-0 needs to be initted to a caosNoop
		std::vector<shared_ptr<caosOp> > allOps;

		script() : variant(NULL) {} // Deserialization only
	public:
		int fmly, gnus, spcs, scrp;
		Variant *variant;
		const Variant *getVariant() const { return variant; };
		
		std::string filename;

		caosOp *getOp(int idx) const {
			assert (idx >= 0);
			if ((unsigned int) idx >= allOps.size())
				return NULL;
			else
				return allOps[idx].get();
		}
		
		std::map<std::string, int> gsub;
		int getNextIndex() { return allOps.size(); }
		// add op as the next opcode
		void thread(caosOp *op);
		script(Variant *v, const std::string &fn,
				int fmly_, int gnus_, int spcs_, int scrp_);
		script(Variant *v, const std::string &fn);
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
BOOST_CLASS_EXPORT(script)


class caosScript { //: Collectable {
public:
	Variant *v;
	std::string filename;
	shared_ptr<script> installer, removal;
	std::vector<shared_ptr<script> > scripts;
	shared_ptr<script> current;

	caosScript(const std::string &variant, const std::string &fn);
	void parse(std::istream &in);
	~caosScript();
	void installScripts();

	caosScript() : v(NULL) {} // For deserialization only
private:
friend class boost::serialization::access;
template<class Archive>
	void load(Archive &ar, unsigned long version) {
		std::string vname;
		ar & vname;
		if (variants.find(vname) == variants.end())
			abort(); // XXX
		v = variants[vname];
		ar & filename & installer & removal
		   & scripts  & current;
	}
template<class Archive>
	void save(Archive &ar, unsigned long version) const {
		ar & v->name;
		ar & filename & installer & removal
		   & scripts  & current;
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()


};
BOOST_CLASS_EXPORT(caosScript)
#endif
/* vim: set noet: */
