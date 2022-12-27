#pragma once

#include "ObjectHandle.h"
#include "common/Exception.h"

#include <array>
#include <map>
#include <stdint.h>
#include <string>

struct ShortToken {
	explicit ShortToken()
		: data{0, 0} {}
	explicit ShortToken(const char s[2])
		: data{s[0], s[1]} {}
	explicit ShortToken(const char (&s)[2])
		: data{s[0], s[1]} {}
	bool operator==(ShortToken other) const {
		return data == other.data;
	}
	std::array<char, 2> data;
};

struct Token {
	explicit Token()
		: data{0, 0, 0, 0} {}
	explicit Token(const char s[4])
		: data{s[0], s[1], s[2], s[3]} {}
	explicit Token(const char (&s)[4])
		: data{s[0], s[1], s[2], s[3]} {}
	bool operator<(Token other) const {
		return *reinterpret_cast<const uint32_t*>(data.data()) < *reinterpret_cast<const uint32_t*>(other.data.data());
	}
	bool operator==(Token other) const {
		return data == other.data;
	}
	std::array<char, 4> data;
};

std::string repr(ShortToken i);
std::string repr(Token i);

class MacroContext;
struct Macro;

using CommandFunc = void (*)(MacroContext&, Macro&);
using AgentRVFunc = ObjectHandle (*)(MacroContext&, Macro&);
using IntegerRVFunc = int32_t (*)(MacroContext&, Macro&);
using LValueFunc = void (*)(const MacroContext&, const Macro&, int32_t value);

class EventManager;
class Object;
class ObjectManager;
class Renderable;
class RenderableManager;
class C1SoundManager;

class MacroError : public Exception {
	using Exception::Exception;
};

class UnknownMacroCommand : public MacroError {
	using MacroError::MacroError;
};

class UnexpectedEndOfMacro : public MacroError {
	using MacroError::MacroError;
};

class MacroContext {
  public:
	void tick_macro(Macro&);

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
	Object* maybe_get_ownr(const Macro&) const;
	Object* get_targ(const Macro&) const;
	Renderable* get_targ_part(const Macro&) const;
	void set_targ(Macro&, ObjectHandle);

	bool try_get_variable(const Macro&, Token varname, int32_t* value) const;
	void set_variable(Macro&, Token varname, int32_t value);

	bool debug = false;
	int32_t instructions_left_this_tick = 0;

	std::map<Token, CommandFunc> command_funcs;
	std::map<Token, AgentRVFunc> agentrv_funcs;
	std::map<Token, IntegerRVFunc> integerrv_funcs;
	std::map<Token, LValueFunc> lvalue_funcs;
};