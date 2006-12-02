/*
 *  bytecode.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
 *  Copyright (c) 2005-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
#ifndef BYTECODE_H
#define BYTECODE_H 1

#include "caosVM.h"
#include "lexutil.h"
#include "cmddata.h"
#include "caosScript.h"
#include "Agent.h"
#include <cstdio>

#include <sstream>
#include <boost/format.hpp>

using boost::str; // boost::format convertor

class script;

typedef void (caosVM::*ophandler)();
class caosOp {
	public:
		// on entry vm->nip = our position + 1
		virtual void execute(caosVM *vm) {
			if (!vm->inst) vm->timeslice -= evalcost;
		}
		void setCost(int cost) {
			evalcost = cost;
		}
		virtual void relocate(const std::vector<int> &relocations) {}
		caosOp() : evalcost(0), owned(false), yyline(lex_lineno) {}
		virtual ~caosOp() {};
		virtual std::string dump() = 0;
		int getlineno() const { return yyline; }
		int getIndex()  const { return index;  }
	protected:
		FRIEND_SERIALIZE(caosOp);
		int index;
		int evalcost;
		bool owned; // if it's been threaded
		int yyline; // HORRIBLE HACK
		friend void script::thread(caosOp *op);
};

class caosNoop : public caosOp {
	public:
		std::string dump() { return std::string("noop"); }
};

class caosJMP : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosJMP);
		int p;
		caosJMP() {}
	public:
		caosJMP(int p_) : p(p_) { }
		void execute(caosVM *vm) { vm->nip = p; }
		void relocate(const std::vector<int> &relocations) {
			if (p < 0)
				p = relocations[-p];
			assert (p > 0);
		}
		std::string dump() { 
			return str(boost::format("JMP %08d") % p);
		}
};

class caosCJMP : public caosJMP {
	FRIEND_SERIALIZE(caosCJMP);
	public:
		caosCJMP(int p_) : caosJMP(p_) {}
		void execute(caosVM *vm) {
			VM_PARAM_INTEGER(flag);

			if (flag)
				this->caosJMP::execute(vm);
		}
		std::string dump() {
			return std::string("COND ") + this->caosJMP::dump();
		}
};

class simpleCaosOp : public caosOp {
	protected:
		FRIEND_SERIALIZE(simpleCaosOp);
		const cmdinfo *ci;
		simpleCaosOp() {}
	public:
		simpleCaosOp(const cmdinfo *i) : ci(i) { evalcost = ci->evalcost; }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			int stackc = vm->valueStack.size();
			dispatchCAOS(vm, ci->disp_id);
			int delta = vm->valueStack.size() - stackc;
			if (!vm->result.isNull())
				delta++;
			if (ci->retc != -1 && 
					delta != ci->retc - ci->argc) {
				std::ostringstream oss;
				oss << "return count mismatch for op "
					<< ci->fullname << ", delta=" << delta;
				throw caosException(oss.str());
			}
		}
		std::string dump() {
			return std::string(ci->fullname);
		}
};

class caosREPS : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosREPS);
		int exit;
		caosREPS() {}
	public:
		caosREPS(int exit_) : exit(exit_) {}
		void relocate(const std::vector<int> &relocations) {
			if (exit < 0) {
				exit = relocations[-exit];
			}
			assert(exit > 0);
		}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			VM_PARAM_INTEGER(i)
			caos_assert(i >= 0);
			if (i == 0) {
				vm->nip = exit;
				return;
			}
			vm->result.setInt(i - 1);
		}
		std::string dump() { return std::string("REPS"); }
};
		
class caosGSUB : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosGSUB);
		int targ;
		caosGSUB () {}
	public:
		caosGSUB(int targ_) : targ(targ_) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			callStackItem i;
			i.valueStack =
				boost::shared_ptr<std::vector<vmStackItem> >(
						new std::vector<vmStackItem>
				);
			i.valueStack->swap(vm->valueStack);
			i.nip = vm->nip;
			vm->callStack.push_back(i);
			vm->valueStack.clear();
			vm->nip = targ;
		}
		void relocate(const std::vector<int> &relocations) {
			if (targ < 0) {
				targ = relocations[-targ];
			}
			assert(targ > 0);
		}

		std::string dump() { 
			return str(boost::format("GSUB %08d") % targ);
		}
};

// Condition classes
#define CEQ 1
#define CLT 2
#define CGT 4
#define CMASK (CEQ | CLT | CGT)
#define CLE (CEQ | CLT)
#define CGE (CEQ | CGT)
#define CNE (CLT | CGT)

extern const char *cnams[];

class caosCond : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosCond);
		int cond;
		bool isAnd;
		caosCond() {}
	public:
		std::string dump() { 
			std::ostringstream oss;
			const char *c, *andMark;
			if (cond < CMASK && cond > 0)
				c = cnams[cond];

			andMark = isAnd ? "AND" : "OR";

			if (c)
				return str(boost::format("COND %s %s") % c % andMark);
			else
				return str(boost::format("COND BAD %d %s") % cond % andMark);
		}
				
			
		caosCond(int condition, bool ia)
			: cond(condition), isAnd(ia) {}

		void execute(caosVM *vm) {
			caosOp::execute(vm);
			
			VM_PARAM_VALUE(arg2);
			VM_PARAM_VALUE(arg1);
			VM_PARAM_INTEGER(flag);

			int cres;
			if (arg2.hasString() && arg1.hasString()) {
				std::string str1 = arg1.getString();
				std::string str2 = arg2.getString();
				
				if (str1 < str2)
					cres = CLT;
				else if (str1 > str2)
					cres = CGT;
				else
					cres = CEQ;
			} else if (arg2.hasInt() && arg1.hasInt()) {
				int val1 = arg1.getInt();
				int val2 = arg2.getInt();

				if (val1 < val2)
					cres = CLT;
				else if (val1 > val2)
					cres = CGT;
				else
					cres = CEQ;
			} else if (arg2.hasDecimal() && arg1.hasDecimal()) {
				float val1 = arg1.getFloat();
				float val2 = arg2.getFloat();

				if (val1 < val2)
					cres = CLT;
				else if (val1 > val2)
					cres = CGT;
				else
					cres = CEQ;
			} else if (arg2.hasAgent() && arg1.hasAgent()) {
				if (cond != CEQ && cond != CNE)
					throw caosException("invalid comparison for agents");
				boost::shared_ptr<Agent> a1, a2;
				a1 = arg1.getAgent();
				a2 = arg2.getAgent();
				if (a1 == a2)
					cres = CEQ;
				else
					cres = CNE;
				// the next bit is needed for some missing GAME etc
			} else cres = CNE;

			bool success = cres & cond;
			if (isAnd) {
				flag = flag && success;
			} else {
				flag = flag || success;
			}

			vm->result.setInt(flag);
		}
};

class caosENUM_POP : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosENUM_POP);
		int exit;
		caosENUM_POP() {}
	public:
		caosENUM_POP(int exit_) : exit(exit_) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			VM_PARAM_VALUE(v);
			if (v.isNull()) { // no more values
				vm->nip = exit;
				vm->targ = vm->owner;
				return;
			}
			if (v.getAgent() == NULL) { // killed?
				vm->nip = vm->cip;
				return;
			}
			vm->setTarg(v.getAgent());
		}
		void relocate(const std::vector<int> &relocations) {
			if (exit < 0) {
				exit = relocations[-exit];
			}
			assert(exit > 0);
		}
		std::string dump() {
			return str(boost::format("ENUM_POP %08d") % exit);
		}
};

class caosSTOP : public caosOp {
	public:
		void execute(caosVM *vm) {
			vm->stop();
		}
		std::string dump() {
			return std::string("STOP");
		}
};

class caosAssert : public caosOp {
	public:
		void execute(caosVM *vm) {
			throw caosException("DBG: ASRT failed");
		}
		std::string dump() {
			return std::string("ASSERT FAILURE");
		}
};

class caosFACE : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosFACE);
		enum ci_type t;
		caosFACE() {}
	public:
		caosFACE (enum ci_type t_) : t(t_) {}
		void execute(caosVM *vm) {
			switch (t) {
				case CI_STRING: vm->s_FACE(); break;
				default:        vm->v_FACE(); break;
			}
		}
		virtual std::string dump() { return std::string("FACE"); }
};

class opBytestr : public caosOp {
	protected:
		FRIEND_SERIALIZE(opBytestr);
		bytestring_t bytestr;
		opBytestr () {}
	public:
		opBytestr(bytestring_t bs) : bytestr(bs) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			vm->valueStack.push_back(bytestr);
		}

		std::string dump() {
			std::ostringstream oss;
			oss << "BYTESTR [ ";
			for (unsigned int i = 0; i < bytestr.size(); i++) {
				oss << (int)bytestr[i] << " ";
			}
			oss << "]";
			return oss.str();
		}
};

class ConstOp : public caosOp {
	protected:
		FRIEND_SERIALIZE(ConstOp);
		caosVar constVal;
		ConstOp() {}
	public:
		virtual void execute(caosVM *vm) {
			vm->valueStack.push_back(constVal);
			caosOp::execute(vm);
		}

		ConstOp(const caosVar &val) {
			constVal = val;
		}

		std::string dump() {
			return std::string("CONST ") + constVal.dump();
		}

};

class opVAxx : public caosOp {
	protected:
		FRIEND_SERIALIZE(opVAxx);
		int index;
		opVAxx() {}
	public:
		opVAxx(int i) : index(i) { assert(i >= 0 && i < 100); }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			vm->valueStack.push_back(&vm->var[index]);
		}

		std::string dump() {
			return str(boost::format("VA%02d") % index);
		}
};

class opOVxx : public caosOp {
	protected:
		FRIEND_SERIALIZE(opOVxx);
		int index;
		opOVxx() {}
	public:
		opOVxx(int i) : index(i) { assert(i >= 0 && i < 100); }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			caos_assert(vm->targ);
			vm->valueStack.push_back(&vm->targ->var[index]);
		}
		std::string dump() {
			return str(boost::format("OV%02d") % index);
		}
};

class opMVxx : public caosOp {
	protected:
		FRIEND_SERIALIZE(opMVxx);
		int index;
		opMVxx() {}
	public:
		opMVxx(int i) : index(i) { assert(i >= 0 && i < 100); }
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			caos_assert(vm->owner);
			vm->valueStack.push_back(&vm->owner->var[index]);
		}
		std::string dump() {
			return str(boost::format("MV%02d") % index);
		}
};


#endif

/* vim: set noet: */
