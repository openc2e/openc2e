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

class EventManager;
class Object;
class ObjectManager;
class Renderable;
class RenderableManager;
class C1SoundManager;

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
	SCRIPT_PICKUP = 4,
	SCRIPT_DROP = 5,
	SCRIPT_COLLISION = 6,
	SCRIPT_INITIALIZE = 7,
	SCRIPT_DEINITIALIZE = 8,
	SCRIPT_TIMER = 9,

	// creature
	SCRIPT_EXTRA_QUIESCENT = 16,
	SCRIPT_EXTRA_ACTIVATE1 = 17,
	SCRIPT_EXTRA_ACTIVATE2 = 18,
	SCRIPT_EXTRA_DEACTIVATE = 19,
	SCRIPT_EXTRA_SEEK = 20,
	SCRIPT_EXTRA_AVOID = 21,
	SCRIPT_EXTRA_PICKUP = 22,
	SCRIPT_EXTRA_DROP = 23,
	SCRIPT_EXTRA_NEED = 24,
	SCRIPT_EXTRA_REST = 25,
	SCRIPT_EXTRA_WEST = 26,
	SCRIPT_EXTRA_EAST = 27,
	SCRIPT_EXTRA_UNDEFINED1 = 28,
	SCRIPT_EXTRA_UNDEFINED2 = 29,
	SCRIPT_EXTRA_UNDEFINED3 = 30,
	SCRIPT_EXTRA_UNDEFINED4 = 31,

	// - introspective
	SCRIPT_INTRO_QUIESCENT = 32,
	SCRIPT_INTRO_ACTIVATE1 = 33,
	SCRIPT_INTRO_ACTIVATE2 = 34,
	SCRIPT_INTRO_DEACTIVATE = 35,
	SCRIPT_INTRO_SEEK = 36,
	SCRIPT_INTRO_AVOID = 37,
	SCRIPT_INTRO_PICKUP = 38,
	SCRIPT_INTRO_DROP = 39,
	SCRIPT_INTRO_NEED = 40,
	SCRIPT_INTRO_REST = 41,
	SCRIPT_INTRO_WEST = 42,
	SCRIPT_INTRO_EAST = 43,
	SCRIPT_INTRO_UNDEFINED1 = 44,
	SCRIPT_INTRO_UNDEFINED2 = 45,
	SCRIPT_INTRO_UNDEFINED3 = 46,
	SCRIPT_INTRO_UNDEFINED4 = 47,

	// pointer
	SCRIPT_POINTER_ACTIVATE1 = 50,
	SCRIPT_POINTER_ACTIVATE2 = 51,
	SCRIPT_POINTER_DEACTIVATE = 52,
	SCRIPT_POINTER_PICKUP = 53,
	SCRIPT_POINTER_DROP = 54,

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
std::string repr(ShortToken i);
std::string repr(Token i);
inline constexpr ShortToken shorttoken(const char s[2]) {
	return static_cast<ShortToken>(s[0] | (s[1] << 8));
}
inline constexpr Token token(const char s[4]) {
	return static_cast<Token>(s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24));
}

class MacroContext;

using CommandFunc = void (*)(MacroContext&, Macro&);
using AgentRVFunc = ObjectHandle (*)(MacroContext&, Macro&);
using IntegerRVFunc = int32_t (*)(const MacroContext&, const Macro&);
using LValueFunc = void (*)(const MacroContext&, const Macro&, int32_t value);

class MacroContext {
  public:
	ObjectHandle read_object(Macro&);

	void read_command_separator(Macro&);
	void read_arg_separator(Macro&);
	bool read_condition(Macro&);
	int32_t read_int(Macro&);
	ShortToken read_short_token(Macro&);
	std::string read_bracket_string(Macro&);
	Token read_token(Macro&);
	std::string read_filename_token(Macro&);

	Object* get_ownr(const Macro&) const;
	Object* get_targ(const Macro&) const;
	Renderable* get_targ_part(const Macro&) const;

	bool try_get_variable(const Macro&, Token varname, int32_t* value) const;
	void set_variable(Macro&, Token varname, int32_t value);

	bool debug = false;
	int32_t instructions_left_this_tick = 0;

	ObjectManager* objects = nullptr;
	RenderableManager* renderables = nullptr;
	EventManager* events = nullptr;
	C1SoundManager* sounds = nullptr;

	std::map<Token, CommandFunc> command_funcs;
	std::map<Token, AgentRVFunc> agentrv_funcs;
	std::map<Token, IntegerRVFunc> integerrv_funcs;
	std::map<Token, LValueFunc> lvalue_funcs;
};

class MacroManager {
  public:
	void add(Macro macro);
	void tick();

	// private:
	MacroContext ctx;
	std::vector<Macro> m_pool;
};