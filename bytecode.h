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
		caosOp() : evalcost(1), owned(false), yyline(lex_lineno) {}
		virtual ~caosOp() {};
		virtual std::string dump() { return "UNKNOWN"; }
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
		caosNoop() { evalcost = 0; }
		std::string dump() { return std::string("noop"); }
};

class caosJMP : public caosOp {
	protected:
		FRIEND_SERIALIZE(caosJMP);
		int p;
		caosJMP() {}
	public:
		caosJMP(int p_) : p(p_) { evalcost = 0; }
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

class simpleCaosOp : public caosOp {
	protected:
		FRIEND_SERIALIZE(simpleCaosOp);
		const cmdinfo *ci;
		simpleCaosOp() {}
	public:
		simpleCaosOp(const cmdinfo *i) : ci(i) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			int stackc = vm->valueStack.size();
			(vm->*(ci->handler))();
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
			i.valueStack = vm->valueStack; // XXX: a bit slow?
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
		int branch;
		caosCond() {}
	public:
		std::string dump() { 
			std::ostringstream oss;
			const char *c;
			if (cond < CMASK && cond > 0)
				c = cnams[cond];
			if (c)
				return str(boost::format("COND %s %08d") % c % branch);
			else
				return str(boost::format("COND BAD %d %08d") % cond % branch);
		}
				
			
		caosCond(int condition, int br)
			: cond(condition), branch(br) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			
			VM_PARAM_VALUE(arg2);
			VM_PARAM_VALUE(arg1);

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
				Agent *a1, *a2;
				a1 = arg1.getAgent();
				a2 = arg2.getAgent();
				if (a1 == a2)
					cres = CEQ;
				else
					cres = CNE;
				// the next bit is needed for some missing GAME etc
			} else cres = CNE;

			if (cres & cond)
				vm->nip = branch;
		}

		void relocate(const std::vector<int> &relocations) {
			if (branch < 0) {
				branch = relocations[-branch];
			}
			assert(branch > 0);
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
};

class opBytestr : public caosOp {
	protected:
		FRIEND_SERIALIZE(opBytestr);
		std::vector<unsigned int> bytestr;
		opBytestr () {}
	public:
		opBytestr(const std::vector<unsigned int> &bs) : bytestr(bs) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			vm->valueStack.push_back(bytestr);
		}

		std::string dump() {
			std::ostringstream oss;
			oss << "BYTESTR [ ";
			for (unsigned int i = 0; i < bytestr.size(); i++) {
				oss << i << " ";
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
		opVAxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
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
		opOVxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
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
		opMVxx(int i) : index(i) { assert(i >= 0 && i < 100); evalcost = 0; }
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
