#pragma once

#include "ObjectHandle.h"
#include "common/Ascii.h"
#include "common/StaticVector.h"

#include <array>
#include <fmt/core.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Object;
class ObjectManager;
class Renderable;
class RenderableManager;

enum MessageNumber {
	MESSAGE_ACTIVATE1 = 0,
	MESSAGE_ACTIVATE2 = 1,
	MESSAGE_DEACTIVATE = 2,
	MESSAGE_HIT = 3,
	MESSAGE_PICKUP = 4,
	MESSAGE_DROP = 5,
};

enum ScriptNumber {
	SCRIPT_DEACTIVATE = 0,
	SCRIPT_ACTIVATE1 = 1,
	SCRIPT_ACTIVATE2 = 2,
	SCRIPT_HIT = 3,
	SCRIPT_PICKEDUP = 4,
	SCRIPT_DROPPED = 5,
	SCRIPT_COLLISION = 6,
	SCRIPT_INITIALIZE = 7,
	SCRIPT_DEINITIALIZE = 8,
	SCRIPT_TIMER = 9,

	// creature
	SCRIPT_EXTRAQUIESCENT = 16,
	SCRIPT_EXTRAACT1 = 17,
	SCRIPT_EXTRAACT2 = 18,
	SCRIPT_EXTRADEAC = 19,
	SCRIPT_EXTRASEEK = 20,
	SCRIPT_EXTRAAVOID = 21,
	SCRIPT_EXTRAPICKUP = 22,
	SCRIPT_EXTRADROP = 23,
	SCRIPT_EXTRANEED = 24,
	SCRIPT_EXTRAREST = 25,
	SCRIPT_EXTRAWEST = 26,
	SCRIPT_EXTRAEAST = 27,
	SCRIPT_EXTRAUNDEFINED1 = 28,
	SCRIPT_EXTRAUNDEFINED2 = 29,
	SCRIPT_EXTRAUNDEFINED3 = 30,
	SCRIPT_EXTRAUNDEFINED4 = 31,

	// - introspective
	SCRIPT_INTROQUIESCENT = 32,
	SCRIPT_INTROACT1 = 33,
	SCRIPT_INTROACT2 = 34,
	SCRIPT_INTRODEAC = 35,
	SCRIPT_INTROSEEK = 36,
	SCRIPT_INTROAVOID = 37,
	SCRIPT_INTROPICKUP = 38,
	SCRIPT_INTRODROP = 39,
	SCRIPT_INTRONEED = 40,
	SCRIPT_INTROREST = 41,
	SCRIPT_INTROWEST = 42,
	SCRIPT_INTROEAST = 43,
	SCRIPT_INTROUNDEFINED1 = 44,
	SCRIPT_INTROUNDEFINED2 = 45,
	SCRIPT_INTROUNDEFINED3 = 46,
	SCRIPT_INTROUNDEFINED4 = 47,

	// pointer
	SCRIPT_POINTERACT1 = 50,
	SCRIPT_POINTERACT2 = 51,
	SCRIPT_POINTERDEAC = 52,
	SCRIPT_POINTERPICKUP = 53,
	SCRIPT_POINTERDROP = 54,

	// involuntary
	SCRIPT_INVOLUNTARY0 = 64,
	SCRIPT_INVOLUNTARY1 = 65,
	SCRIPT_INVOLUNTARY2 = 66,
	SCRIPT_INVOLUNTARY3 = 67,
	SCRIPT_INVOLUNTARY4 = 68,
	SCRIPT_INVOLUNTARY5 = 69,
	SCRIPT_INVOLUNTARY6 = 70,
	SCRIPT_INVOLUNTARY7 = 71,
	SCRIPT_DIE = 72,
};

class Macro {
  public:
	bool selfdestruct = false;
	bool inst = false;
	std::string script;
	uint32_t ip = 0;
	StaticVector<int32_t, 20> stack;
	std::array<int32_t, 10> vars;
	ObjectHandle ownr;
	ObjectHandle from;
	ObjectHandle exec;
	ObjectHandle targ;
	ObjectHandle _it_;
	uint32_t part = 0;
	std::string subroutine_label;
	uint32_t subroutine_address = 0;
	uint32_t wait = 0;
};

enum class ShortToken : uint16_t {};

enum class Token : uint32_t {};

class MacroContext;

using CommandFunc = void (*)(MacroContext&, Macro&);
using AgentRVFunc = ObjectHandle (*)(MacroContext&, Macro&);
using IntegerRVFunc = int32_t (*)(const MacroContext&, const Macro&);

class MacroContext {
  public:
	MacroContext(ObjectManager& objects_, RenderableManager& renderables_);
	ObjectHandle read_object(Macro&);

	void read_command_separator(Macro&);
	void read_arg_separator(Macro&);
	bool read_condition(Macro&);
	int32_t read_int(Macro&);
	ShortToken read_short_token(Macro&);
	std::string read_bracket_string(Macro&);
	Token read_token(Macro&);

	Object* get_ownr(const Macro&) const;
	Object* get_targ(const Macro&) const;
	Renderable* get_targ_part(const Macro&) const;

	bool try_get_variable(const Macro&, Token varname, int32_t* value) const;
	void set_variable(Macro&, Token varname, int32_t value);

	bool debug = false;

	ObjectManager& objects;
	RenderableManager& renderables;
	std::map<Token, CommandFunc> command_funcs;
	std::map<Token, IntegerRVFunc> integerrv_funcs;
	std::map<Token, AgentRVFunc> agentrv_funcs;
};

class MacroManager {
  public:
	MacroManager(std::shared_ptr<ObjectManager>, std::shared_ptr<RenderableManager>);
	void add(Macro macro);
	void tick();

	// private:
	std::vector<Macro> m_pool;
	std::shared_ptr<ObjectManager> m_objects;
	std::shared_ptr<RenderableManager> m_renderables;
};