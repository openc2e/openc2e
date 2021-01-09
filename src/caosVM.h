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

#ifndef _CAOSVM_H
#define _CAOSVM_H

#include "caos_assert.h"
#include "bytestring.h"
#include <array>
#include <iosfwd>
#include <memory>
#include "AgentRef.h"
#include "caosValue.h"
#include "alloc_count.h"
#include <type_traits>

class script;

//#define CAOSDEBUG
//#define CAOSDEBUGDETAIL

// caosVM_agent.cpp:
unsigned int calculateScriptId(unsigned int message_id);

#define LVAL 1
#define RVAL 2
#define BYTESTR 4

class badParamException : public caosException {
	public:
		badParamException() : caosException("parameter type mismatch") {}
};

struct callStackItem {
	std::vector<caosValue> valueStack;
	int nip;
};

typedef class caosVM *caosVM_p;

class blockCond {
	// XXX NOT SERIALIZABLE FIXME
	public:
		virtual bool operator()() = 0;
		virtual ~blockCond() {}
};

class caosVM {
	COUNT_ALLOC(caosVM)
public:	
	int trace;

	blockCond *blocking;

	void startBlocking(blockCond *whileWhat);
	bool isBlocking();
	
	// nb, ptr is immutable, class is mutable
	// This is so the stack manipulation macros work in the op classes as well
	const caosVM_p vm; // == this
	
	// script state...
	std::shared_ptr<script> currentscript;
	int nip, cip, runops;
	
	bool inst, lock, stop_loop;
	int timeslice;

	std::vector<caosValue> valueStack;
	std::vector<caosValue> auxStack;
	std::vector<callStackItem> callStack;
	
	std::istream *inputstream;
	std::ostream *outputstream;

	// ...which includes variables accessible to script
	std::array<caosValue, 100> var;
	caosValue _p_[2]; // might want to add this onto the end of above map, if done
	AgentRef targ, owner, _it_;
	caosValue from;
	int part;
	std::weak_ptr<class Camera> camera;
	class Camera *getCamera();
	
	void resetScriptState(); // resets everything except OWNR

public:
	void resetCore();

	caosValue result;

	void setTarg(const AgentRef &a) { targ = a; }
	void setVariables(caosValue &one, caosValue &two) { _p_[0] = one; _p_[1] = two; }
	void setOwner(Agent *a) { owner = a; }
	void setOutputStream(std::ostream &o) { outputstream = &o; }

	class CreatureAgent *getTargCreatureAgent();
	class Creature *getTargCreature();
	class SpritePart *getCurrentSpritePart();
	class AnimatablePart *getCurrentAnimatablePart();

	void safeJMP(int nip);
	void invoke_cmd(script *s, bool is_saver, int opidx);
	void runOpCore(script *s, struct caosOp op);
	void runOp();
	void runEntirely(std::shared_ptr<script> s);

	void tick();
	void stop();
	bool fireScript(std::shared_ptr<script> s, bool nointerrupt, Agent *frm = 0);

	caosVM(const AgentRef &o);
	~caosVM();

	bool stopped() { return !currentscript; }
};

class notEnoughParamsException : public caosException {
	public:
		notEnoughParamsException() : caosException("Not enough parameters") {}
};

class invalidAgentException : public caosException {
	public:
		invalidAgentException() : caosException("Invalid agent handle") {}
		invalidAgentException(const char *s) : caosException(s) {}
		invalidAgentException(const std::string &s) : caosException(s) {}
};

#define VM_VERIFY_SIZE(n) // no-op, we assert in the pops. orig: if (params.size() != n) { throw notEnoughParamsException(); }
static inline void VM_STACK_CHECK(const caosVM *vm) {
	if (!vm->valueStack.size())
		throw notEnoughParamsException();
}

class caosVM__lval {
	protected:
		caosVM *owner;
	public:
		caosValue value;
		caosVM__lval(caosVM *vm) : owner(vm) {
			VM_STACK_CHECK(vm);
			value = owner->valueStack.back();
			owner->valueStack.pop_back();
		}
		~caosVM__lval() {
			owner->valueStack.push_back(value);
		}
};

template <typename T> auto vmparamhelper(const caosValue& val);
template <> inline auto vmparamhelper<caosValue>(const caosValue& val) { return val; }
template <> inline auto vmparamhelper<std::string>(const caosValue& val) { return val.getString(); }
template <> inline auto vmparamhelper<int>(const caosValue& val) { return val.getInt(); }
template <> inline auto vmparamhelper<float>(const caosValue& val) { return val.getFloat(); }
template <> inline auto vmparamhelper<Vector<float>>(const caosValue& val) { return val.getVector(); }
template <> inline auto vmparamhelper<std::shared_ptr<Agent>>(const caosValue& val) { return val.getAgent(); }
template <> inline auto vmparamhelper<AgentRef>(const caosValue& val) { return val.getAgent(); }

#define VM_PARAM_OF_TYPE(name, type) \
	type name; { VM_STACK_CHECK(vm); caosValue __x = vm->valueStack.back(); \
		name = vmparamhelper<type>(__x); } vm->valueStack.pop_back();

#define VM_PARAM_VALUE(name) VM_PARAM_OF_TYPE(name, caosValue)
#define VM_PARAM_STRING(name) VM_PARAM_OF_TYPE(name, std::string)
#define VM_PARAM_INTEGER(name) VM_PARAM_OF_TYPE(name, int)
#define VM_PARAM_FLOAT(name) VM_PARAM_OF_TYPE(name, float)
#define VM_PARAM_VECTOR(name) VM_PARAM_OF_TYPE(name, Vector<float>)
#define VM_PARAM_AGENT(name) VM_PARAM_OF_TYPE(name, std::shared_ptr<Agent>)
// TODO: is usage of valid_agent correct here, or should we be caos_asserting?
#define VM_PARAM_VALIDAGENT(name) VM_PARAM_AGENT(name) valid_agent(name);
#define VM_PARAM_VARIABLE(name) caosVM__lval vm__lval_##name(vm); caosValue * const name = &vm__lval_##name.value;
#define VM_PARAM_DECIMAL(name) caosValue name; { VM_STACK_CHECK(vm); caosValue __x = vm->valueStack.back(); \
	name = __x; } vm->valueStack.pop_back();
#define VM_PARAM_BYTESTR(name) bytestring_t name; { \
	VM_STACK_CHECK(vm); \
	caosValue __x = vm->valueStack.back(); \
	name = __x.getByteStr(); } vm->valueStack.pop_back();

#define CAOS_LVALUE(name, check, get, set) \
	void v_##name(caosVM *vm) { \
		check; \
		vm->valueStack.push_back(caosValue(get)); \
	} \
	void s_##name(caosVM *vm) { \
		check; \
		VM_PARAM_OF_TYPE(newvalue, std::remove_reference_t<decltype(get)>) \
		set; \
	}

#define CAOS_LVALUE_WITH(name, agent, check, get, set) \
	CAOS_LVALUE(name, valid_agent(agent); check, get, set)

#define CAOS_LVALUE_TARG(name, check, get, set) \
	CAOS_LVALUE_WITH(name, vm->targ, check, get, set)

#define CAOS_LVALUE_WITH_SIMPLE(name, agent, exp) \
	CAOS_LVALUE(name, valid_agent(agent), exp, exp = newvalue)

#define CAOS_LVALUE_SIMPLE(name, exp) \
	CAOS_LVALUE(name, (void)0, exp, exp = newvalue)

#define CAOS_LVALUE_TARG_SIMPLE(name, exp) \
	CAOS_LVALUE_TARG(name, (void)0, exp, exp = newvalue)
#define STUB throw caosException("stub in " __FILE__)

#define valid_agent(x) do { \
		if (!(x)) { \
			throw invalidAgentException("Invalid agent handle: " #x " thrown from " __FILE__ ":" stringify(__LINE__)); \
		} \
	} while(0)

#endif
/* vim: set noet: */
