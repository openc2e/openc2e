#include "MacroManager.h"

#include "ObjectManager.h"
#include "common/Exception.h"
#include "common/Repr.h"

#include <map>


std::string repr(ShortToken i) {
	return std::string(i.data.begin(), i.data.end());
}

std::string repr(Token i) {
	return std::string(i.data.begin(), i.data.end());
}


ShortToken MacroContext::read_short_token(Macro& m) {
	// TODO: assert allowed letters
	const char* s = m.script.c_str() + m.ip;
	ShortToken token({s[0], s[1]});
	m.ip += 2;
	return token;
}

Token MacroContext::read_token(Macro& m) {
	// TODO: assert allowed letters
	const char* s = m.script.c_str() + m.ip;
	Token token({s[0], s[1], s[2], s[3]});
	m.ip += 4;
	return token;
}

std::string MacroContext::read_filename_token(Macro& m) {
	// TODO: assert allowed letters
	auto result = m.script.substr(m.ip, 4);
	m.ip += 4;
	return result;
}

Object* MacroContext::get_ownr(const Macro& m) const {
	auto* ownr = objects->try_get<Object>(m.ownr);
	if (!ownr) {
		throw Exception("ownr is not a valid agent reference");
	}
	return ownr;
}

Object* MacroContext::get_targ(const Macro& m) const {
	auto* targ = objects->try_get<Object>(m.targ);
	if (!targ) {
		throw Exception("targ is not a valid agent reference");
	}
	return targ;
}

Renderable* MacroContext::get_targ_part(const Macro& m) const {
	auto* renderable = renderables->try_get(get_targ(m)->get_part(m.part));
	if (!renderable) {
		throw Exception("part is not a valid part reference");
	}
	return renderable;
}

void MacroContext::read_command_separator(Macro& m) {
	if (m.script[m.ip] == ' ' || m.script[m.ip] == ',' || m.script[m.ip] == '\0') {
		if (m.script[m.ip] != '\0') {
			m.ip++;
		}
	} else {
		throw Exception(fmt::format("Expected whitespace, comma, or eoi after command but got {}", repr(m.script.substr(m.ip))));
	}
}

void MacroContext::read_arg_separator(Macro& m) {
	if (m.script[m.ip] == ' ') {
		m.ip++;
	} else {
		throw Exception(fmt::format("Expected whitespace before next argument but got {}", repr(m.script.substr(m.ip))));
	}
}

std::string MacroContext::read_bracket_string(Macro& m) {
	const char* s = m.script.c_str();
	uint32_t& p = m.ip;

	if (s[p] != '[') {
		throw Exception(fmt::format("Expected '[' but got {}", repr(s[p])));
	}
	p++;
	std::string value;
	while (!(s[p] == ']' || s[p] == '\0')) {
		value += s[p];
		p++;
	}
	if (s[p] == '\0') {
		throw Exception("Unexpected end of input when parsing bracket-string");
	}
	p++;
	return value;
}

int32_t MacroContext::read_int(Macro& m) {
	const char* s = m.script.c_str();
	uint32_t& p = m.ip;

	bool negative = false;
	if (s[p] == '-') {
		negative = true;
		p++;
	}
	if (!is_ascii_digit(s[p])) {
		int32_t variable_value;
		Token tok = read_token(m);
		if (try_get_variable(m, tok, &variable_value)) {
			return variable_value;
		}
		auto it = integerrv_funcs.find(tok);
		if (it != integerrv_funcs.end()) {
			if (debug) {
				fmt::print("read_int calling func {}\n", repr(tok));
			}
			return it->second(*this, m);
		}
		throw Exception(fmt::format("Expected integer but got {}", s + p - 4));
	}
	int32_t value = 0;
	while (is_ascii_digit(s[p])) {
		value = value * 10 + (s[p] - '0');
		p++;
	}
	if (negative) {
		value *= -1;
	}
	if (debug) {
		fmt::print("read_int found integer literal\n");
	}
	return value;
}

bool MacroContext::read_condition(Macro& m) {
	int32_t left = read_int(m);
	read_arg_separator(m);
	ShortToken comparison = read_short_token(m);
	read_arg_separator(m);
	int32_t right = read_int(m);

	if (comparison == ShortToken("gt")) {
		return left > right;
	} else if (comparison == ShortToken("ge")) {
		return left >= right;
	} else if (comparison == ShortToken("lt")) {
		return left < right;
	} else if (comparison == ShortToken("le")) {
		return left <= right;
	} else if (comparison == ShortToken("eq")) {
		return left == right;
	} else {
		throw Exception(fmt::format("Unknown comparison operator {}", repr(comparison)));
	}
}

ObjectHandle MacroContext::read_object(Macro& m) {
	const char* s = m.script.c_str();
	uint32_t& p = m.ip;

	Token tok = read_token(m);
	auto it = agentrv_funcs.find(tok);
	if (it == agentrv_funcs.end()) {
		throw Exception(fmt::format("Expected object, but got {}", repr(s + p)));
	}
	return it->second(*this, m);
};

bool MacroContext::try_get_variable(const Macro& m, Token varname, int32_t* value) const {
	if (varname == Token("var0")) {
		*value = m.vars[0];
	} else if (varname == Token("var1")) {
		*value = m.vars[1];
	} else if (varname == Token("var2")) {
		*value = m.vars[2];
	} else if (varname == Token("var3")) {
		*value = m.vars[3];
	} else if (varname == Token("var4")) {
		*value = m.vars[4];
	} else if (varname == Token("var5")) {
		*value = m.vars[5];
	} else if (varname == Token("var6")) {
		*value = m.vars[6];
	} else if (varname == Token("var7")) {
		*value = m.vars[7];
	} else if (varname == Token("var8")) {
		*value = m.vars[8];
	} else if (varname == Token("var9")) {
		*value = m.vars[9];
	} else if (varname == Token("obv0")) {
		*value = get_targ(m)->obv0;
	} else if (varname == Token("obv1")) {
		*value = get_targ(m)->obv1;
	} else if (varname == Token("obv2")) {
		*value = get_targ(m)->obv2;
	} else {
		return false;
	}
	if (debug) {
		fmt::print("{} -> {}\n", repr(varname), *value);
	}
	return true;
}

void MacroContext::set_variable(Macro& m, Token varname, int32_t value) {
	if (varname == Token("var0")) {
		m.vars[0] = value;
	} else if (varname == Token("var1")) {
		m.vars[1] = value;
	} else if (varname == Token("var2")) {
		m.vars[2] = value;
	} else if (varname == Token("var3")) {
		m.vars[3] = value;
	} else if (varname == Token("var4")) {
		m.vars[4] = value;
	} else if (varname == Token("var5")) {
		m.vars[5] = value;
	} else if (varname == Token("var6")) {
		m.vars[6] = value;
	} else if (varname == Token("var7")) {
		m.vars[7] = value;
	} else if (varname == Token("var8")) {
		m.vars[8] = value;
	} else if (varname == Token("var9")) {
		m.vars[9] = value;
	} else if (varname == Token("obv0")) {
		get_targ(m)->obv0 = value;
	} else if (varname == Token("obv1")) {
		get_targ(m)->obv1 = value;
	} else if (varname == Token("obv2")) {
		get_targ(m)->obv2 = value;
	} else {
		auto it = lvalue_funcs.find(varname);
		if (it != lvalue_funcs.end()) {
			it->second(*this, m, value);
			return;
		}
		throw Exception(fmt::format("Unknown variable {}", repr(varname)));
	}
};

void MacroManager::add(Macro macro) {
	m_pool.push_back(macro);
}

template <typename T>
class Index {
  public:
	Index(std::vector<T>& parent_, size_t idx_)
		: parent(parent_), idx(idx_) {}

	T& operator*() {
		return parent[idx];
	}

	T* operator->() {
		return &parent[idx];
	}

	std::vector<T>& parent;
	size_t idx;
};

void MacroManager::tick() {
	// do something

	// iterate in reverse and using an index, macros might be added/destroyed during iteration
	for (size_t i = m_pool.size(); i > 0; --i) {
		auto m = Index<Macro>(m_pool, i - 1);

		ctx.debug = false;
		if (ctx.debug) {
			fmt::print("DEBUG cls=({}, {}, {}) uid={}\n", ctx.get_ownr(*m)->family, ctx.get_ownr(*m)->genus, ctx.get_ownr(*m)->species, m->ownr);
			fmt::print("macro wait={} - {} !!ip!! {} - subroutine {} {}\n", m->wait, m->script.substr(0, m->ip), m->script.substr(m->ip), m->subroutine_label, m->subroutine_address);
			fmt::print("stack ");
			// for (size_t i = 0;)
			for (auto s : m->stack) {
				fmt::print("{} ", s);
			}
			fmt::print("\n");
		}

		// we don't want to let inst run forever...
		ctx.instructions_left_this_tick = m->inst ? 100 : 1;

		try {
			while (ctx.instructions_left_this_tick > 0) {
				auto original_ip = m->ip;
				ctx.instructions_left_this_tick--;
				try {
					Token command = ctx.read_token(*m);
					if (command == Token("endm") || command == Token("stop")) {
						// remove from list, check selfdestruct
						m->ip = original_ip;
						continue;
					}
					auto it = ctx.command_funcs.find(command);
					if (it == ctx.command_funcs.end()) {
						throw Exception(fmt::format("Unknown command {}", repr(command)));
					}
					it->second(ctx, *m);
				} catch (Exception& e) {
					m->ip = original_ip;
					throw;
				}
			}
		} catch (Exception& e) {
			fmt::print("DEBUG cls=({}, {}, {}) uid={}\n", ctx.get_ownr(*m)->family, ctx.get_ownr(*m)->genus, ctx.get_ownr(*m)->species, m->ownr);
			fmt::print("macro {} !!ip!! {}\n", m->script.substr(0, m->ip), m->script.substr(m->ip));
			fmt::print("error: {}\n", e.what());
		}
	}
}