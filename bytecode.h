#ifndef BYTECODE_H
#define BYTECODE_H 1

#include "caosVM.h"
#include "lexutil.h"
#include "cmddata.h"

class script;

typedef void (caosVM::*ophandler)();
class caosOp {
	public:
		virtual void execute(caosVM *vm) {
			vm->timeslice -= evalcost;
			vm->nip = successor;
		}
		void setSuccessor(caosOp *succ) {
			successor = succ;
		}
		void setCost(int cost) {
			evalcost = cost;
		}
		caosOp() : evalcost(1), successor(NULL), owned(false), yyline(lex_lineno) {}
		virtual ~caosOp() {};
		int getlineno() const { return yyline; }
	protected:
		int yyline; // HORRIBLE HACK
		int evalcost;
		caosOp *successor;
		bool owned; // if it's been addOp()ed
		friend void script::addOp(caosOp *op);
};

class caosNoop : public caosOp {
	public:
		caosNoop() { evalcost = 0; }
};

class simpleCaosOp : public caosOp {
	protected:
		const cmdinfo *ci;
		ophandler handler;
	public:
		simpleCaosOp(ophandler h, const cmdinfo *i) : handler(h), ci(i) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			int stackc = vm->valueStack.size();
			(vm->*handler)();
			int delta = vm->valueStack.size() - stackc;
			if (!vm->result.isNull())
				delta++;
			if (ci->retc != -1 && 
					delta != ci->retc - ci->argc) {
				std::cerr << "Warning: return count mismatch for op "
					<< ci->fullname << ", delta=" << delta
					<< std::endl;
			}
		}
};

class caosREPS : public caosOp {
	protected:
		caosOp *exit;
	public:
		caosREPS(caosOp *exit_) : exit(exit_) {}
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
};
		
class caosGSUB : public caosOp {
	protected:
		caosOp *targ;
	public:
		caosGSUB(caosOp *targ_) : targ(targ_) {}
		void execute(caosVM *vm) {
			caosOp::execute(vm);
			callStackItem i;
			i.valueStack = vm->valueStack; // XXX: a bit slow?
			i.nip = vm->nip;
			vm->callStack.push_back(i);
			vm->valueStack.clear();
			vm->nip = targ;
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

class caosCond : public caosOp {
	protected:
		int cond;
		caosOp *branch;
	public:
		caosCond(int condition, caosOp *br)
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
};

class caosENUM_POP : public caosOp {
	protected:
		caosOp *exit;
	public:
		caosENUM_POP(caosOp *exit_) : exit(exit_) {}
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
};

#endif

/* vim: set noet: */
