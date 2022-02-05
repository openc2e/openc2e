#include "MacroManager.h"

#include "ObjectManager.h"
#include "common/Exception.h"
#include "common/Random.h"
#include "common/Repr.h"

#include <fmt/core.h>
#include <map>


static std::string repr(ShortToken i) {
	char s[2];
	s[0] = static_cast<uint16_t>(i);
	s[1] = static_cast<uint16_t>(i) >> 8;
	return std::string(s, s + 2);
}

static std::string repr(Token i) {
	char s[4];
	s[0] = static_cast<uint32_t>(i);
	s[1] = static_cast<uint32_t>(i) >> 8;
	s[2] = static_cast<uint32_t>(i) >> 16;
	s[3] = static_cast<uint32_t>(i) >> 24;
	return std::string(s, s + 4);
}

MacroContext::MacroContext(ObjectManager& objects_, RenderableManager& renderables_)
	: objects(objects_), renderables(renderables_) {
}

ShortToken MacroContext::read_short_token(Macro& m) {
	// TODO: assert allowed letters
	ShortToken token = static_cast<ShortToken>(m.script[m.ip] | (m.script[m.ip + 1] << 8));
	m.ip += 2;
	return token;
}

Token MacroContext::read_token(Macro& m) {
	// TODO: assert allowed letters
	const char* s = m.script.c_str() + m.ip;
	Token token = static_cast<Token>(s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24));
	m.ip += 4;
	return token;
}

Object* MacroContext::get_ownr(const Macro& m) const {
	auto* ownr = objects.try_get<Object>(m.ownr);
	if (!ownr) {
		throw Exception("ownr is not a valid agent reference");
	}
	return ownr;
}

Object* MacroContext::get_targ(const Macro& m) const {
	auto* targ = objects.try_get<Object>(m.targ);
	if (!targ) {
		throw Exception("targ is not a valid agent reference");
	}
	return targ;
}

Renderable* MacroContext::get_targ_part(const Macro& m) const {
	auto* renderable = renderables.try_get(get_targ(m)->get_part(m.part));
	if (!renderable) {
		throw Exception("part is not a valid part reference");
	}
	return renderable;
}

static constexpr ShortToken shorttoken(const char s[2]) {
	return static_cast<ShortToken>(s[0] | (s[1] << 8));
}

static constexpr Token token(const char s[4]) {
	return static_cast<Token>(s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24));
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

int32_t IntegerRV_POSB(const MacroContext& ctx, const Macro& m) {
	auto* renderable = ctx.get_targ_part(m);
	if (!renderable) {
		throw Exception("Called POSL on object without any parts");
	}
	int32_t posb = renderable->y + renderable->height();
	if (ctx.debug) {
		printf("POSB -> %i\n", posb);
	}
	return posb;
}

int32_t IntegerRV_POSL(const MacroContext& ctx, const Macro& m) {
	auto* renderable = ctx.get_targ_part(m);
	if (!renderable) {
		throw Exception("Called POSL on object without any parts");
	}
	return renderable->x;
}

int32_t IntegerRV_XVEC(const MacroContext&, const Macro&) {
	printf("WARNING: XVEC not implemented\n");
	return 0;
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
				printf("read_int calling func %s\n", repr(tok).c_str());
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
		printf("read_int found integer literal\n");
	}
	return value;
}

bool MacroContext::read_condition(Macro& m) {
	int32_t left = read_int(m);
	read_arg_separator(m);
	ShortToken comparison = read_short_token(m);
	read_arg_separator(m);
	int32_t right = read_int(m);

	if (comparison == shorttoken("gt")) {
		return left > right;
	} else if (comparison == shorttoken("ge")) {
		return left >= right;
	} else if (comparison == shorttoken("lt")) {
		return left < right;
	} else if (comparison == shorttoken("le")) {
		return left <= right;
	} else if (comparison == shorttoken("eq")) {
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
	if (varname == token("var0")) {
		*value = m.vars[0];
	} else if (varname == token("var1")) {
		*value = m.vars[1];
	} else if (varname == token("var2")) {
		*value = m.vars[2];
	} else if (varname == token("var3")) {
		*value = m.vars[3];
	} else if (varname == token("var4")) {
		*value = m.vars[4];
	} else if (varname == token("var5")) {
		*value = m.vars[5];
	} else if (varname == token("var6")) {
		*value = m.vars[6];
	} else if (varname == token("var7")) {
		*value = m.vars[7];
	} else if (varname == token("var8")) {
		*value = m.vars[8];
	} else if (varname == token("var9")) {
		*value = m.vars[9];
	} else if (varname == token("obv0")) {
		*value = get_targ(m)->obv0;
	} else if (varname == token("obv1")) {
		*value = get_targ(m)->obv1;
	} else if (varname == token("obv2")) {
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
	if (varname == token("var0")) {
		m.vars[0] = value;
	} else if (varname == token("var1")) {
		m.vars[1] = value;
	} else if (varname == token("var2")) {
		m.vars[2] = value;
	} else if (varname == token("var3")) {
		m.vars[3] = value;
	} else if (varname == token("var4")) {
		m.vars[4] = value;
	} else if (varname == token("var5")) {
		m.vars[5] = value;
	} else if (varname == token("var6")) {
		m.vars[6] = value;
	} else if (varname == token("var7")) {
		m.vars[7] = value;
	} else if (varname == token("var8")) {
		m.vars[8] = value;
	} else if (varname == token("var9")) {
		m.vars[9] = value;
	} else if (varname == token("obv0")) {
		get_targ(m)->obv0 = value;
	} else if (varname == token("obv1")) {
		get_targ(m)->obv1 = value;
	} else if (varname == token("obv2")) {
		get_targ(m)->obv2 = value;
	} else {
		throw Exception(fmt::format("Unknown variable {}", repr(varname)));
	}
};

MacroManager::MacroManager(std::shared_ptr<ObjectManager> objects, std::shared_ptr<RenderableManager> renderables)
	: m_objects(objects), m_renderables(renderables) {
}

void MacroManager::add(Macro macro) {
	m_pool.push_back(macro);
}

ObjectHandle AgentRV_TARG(MacroContext&, Macro& macro) {
	return macro.targ;
}

ObjectHandle AgentRV_OWNR(MacroContext&, Macro& macro) {
	return macro.ownr;
}

ObjectHandle AgentRV_FROM(MacroContext&, Macro& macro) {
	return macro.from;
}

void Command_SNDC(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token sound_name = ctx.read_token(m);
	printf("WARNING: SNDC not implemented: %s\n", repr(sound_name).c_str());

	ctx.read_command_separator(m);
}

void Command_SNDE(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token sound_name = ctx.read_token(m);
	printf("WARNING: SNDE not implemented: %s\n", repr(sound_name).c_str());

	ctx.read_command_separator(m);
}

void Command_MVBY(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t xdiff = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t ydiff = ctx.read_int(m);

	auto* obj = ctx.get_targ(m);
	if (auto* comp = dynamic_cast<CompoundObject*>(obj)) {
		throw Exception(fmt::format("Don't know how to MVBY a compound object"));
	} else {
		auto* renderable = ctx.renderables.try_get(obj->get_part(0));
		if (!renderable) {
			throw Exception("Called MVBY on object without any parts");
		}
		renderable->x += xdiff;
		renderable->y += ydiff;
	}

	if (ctx.debug) {
		printf("did a mvby  x=%i y=%i cls=(%i, %i, %i) spr=%s!\n",
			xdiff, ydiff, ctx.get_targ(m)->family, ctx.get_targ(m)->genus, ctx.get_targ(m)->species, ctx.get_targ_part(m)->sprite.getName().c_str());
	}

	ctx.read_command_separator(m);
}

void Command_STIM(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token subcommand = ctx.read_token(m);
	if (subcommand == token("sign")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}

		printf("WARNING: STIM SIGN not implemented\n");

		ctx.read_command_separator(m);

	} else if (subcommand == token("shou")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}

		printf("WARNING: STIM SHOU not implemented\n");

		ctx.read_command_separator(m);

	} else {
		throw Exception(fmt::format("Unknown command 'stim {}'", repr(subcommand)));
	}
}

void Command_TICK(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t tick_value = ctx.read_int(m);
	ctx.get_targ(m)->tick_value = tick_value;
	// TODO: should this be reset?
	ctx.get_targ(m)->ticks_since_last_tick_event = 0;

	ctx.read_command_separator(m);
}

void Command_MESG(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token subcommand = ctx.read_token(m);
	if (subcommand == token("writ")) {
		ctx.read_arg_separator(m);
		ctx.read_object(m);
		ctx.read_arg_separator(m);
		ctx.read_int(m);

		printf("WARNING: MESG WRIT not implemented\n");

		ctx.read_command_separator(m);

	} else {
		throw Exception(fmt::format("Unknown command 'mesg {}'", repr(subcommand)));
	}
}

void Command_ENDI(MacroContext& ctx, Macro& m) {
	// I think we just move on?
	ctx.read_command_separator(m);
}

void Command_RNDV(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	uint32_t low = ctx.read_int(m);
	ctx.read_arg_separator(m);
	uint32_t high = ctx.read_int(m);

	uint32_t generated = Random::randrange(low, high + 1);
	if (ctx.debug) {
		printf("RNDV info %s %i %i\n", repr(varname).c_str(), low, high);
		printf("RNDV generated %i\n", generated);
	}
	ctx.set_variable(m, varname, generated);

	ctx.read_command_separator(m);
}

void Command_ANIM(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	std::string anim_string = ctx.read_bracket_string(m);
	auto* r = ctx.get_targ_part(m);
	r->has_animation = true;
	r->animation_frame = 0;
	r->animation_string = anim_string;

	ctx.read_command_separator(m);
}

void Command_WAIT(MacroContext& ctx, Macro& m) {
	size_t old_ip = m.ip - 4;

	ctx.read_arg_separator(m);
	int32_t wait_value = ctx.read_int(m);
	if (ctx.debug) {
		printf("wait %i\n", wait_value);
	}
	if (m.wait == 0) {
		m.wait = wait_value;
		m.ip = old_ip;
	} else {
		m.wait--;
		if (m.wait == 0) {
			if (ctx.debug) {
				printf("finished waiting!\n");
			}
			ctx.read_command_separator(m);
		} else {
			m.ip = old_ip;
		}
	}
}

void Command_EVER(MacroContext&, Macro& m) {
	// at the end of the loop, go back to start
	m.ip = m.stack.back();
}

void Command_OVER(MacroContext& ctx, Macro& m) {
	if (ctx.get_targ_part(m)->has_animation) {
		m.ip -= 4;
	} else {
		if (ctx.debug) {
			printf("animation over!\n");
		}
		ctx.read_command_separator(m);
	}
}

void Command_REPS(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t iterations = ctx.read_int(m);
	if (iterations <= 0) {
		throw Exception(fmt::format("REPS called with bad value: {}", iterations));
	}
	ctx.read_command_separator(m);
	m.stack.push_back(m.ip);
	m.stack.push_back(iterations);
}

void Command_REPE(MacroContext& ctx, Macro& m) {
	// done with this iteration of loop, go back to start if needed
	auto& iterations = m.stack[m.stack.size() - 1];
	auto& loop_start = m.stack[m.stack.size() - 2];
	if (ctx.debug) {
		printf("repe info iter %i start %i\n", iterations, loop_start);
	}
	iterations--;
	if (iterations > 0) {
		m.ip = loop_start;
	} else {
		m.stack.pop_back(); // iterations
		m.stack.pop_back(); // loop start
		if (ctx.debug) {
			printf("Done with REPE!\n");
		}
		ctx.read_command_separator(m);
	}
}

void Command_DOIF(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	bool cond = ctx.read_condition(m);
	// if condition is true, just go to next command. ELSE will be skipped when
	// we hit it later
	if (cond) {
		if (ctx.debug) {
			printf("DOIF evaluted to true\n");
		}
		ctx.read_command_separator(m);
		return;
	}
	if (ctx.debug) {
		printf("DOIF evaluted to false\n");
	}
	// otherwise, go find the next ELSE/ENDI
	// TODO: hacky and probably wrong! get a better parser
	size_t level = 1;
	while (true) {
		if (m.script[m.ip] == '\0') {
			throw Exception("Unexpected eoi while looking for ELSE/ENDI");
		}
		if (!(m.script[m.ip] == ',' || m.script[m.ip] == ' ')) {
			m.ip++;
			continue;
		}
		m.ip++;
		Token command = ctx.read_token(m);
		if (command == token("doif")) {
			level += 1;
		} else if (command == token("else") && level == 1) {
			// cool, skip to this else
			ctx.read_command_separator(m);
			return;
		} else if (command == token("endi")) {
			level -= 1;
			if (level == 0) {
				// cool, skip to this endi
				ctx.read_command_separator(m);
				return;
			}
		} else {
			m.ip -= 3;
		}
	}
}

void Command_ELSE(MacroContext& ctx, Macro& m) {
	// TODO: hacky and probably wrong! get a better parser

	size_t level = 1;
	while (true) {
		if (m.script[m.ip] == '\0') {
			throw Exception("Unexpected eoi while looking for ENDI");
		}
		if (!(m.script[m.ip] == ',' || m.script[m.ip] == ' ')) {
			m.ip++;
			continue;
		}
		m.ip++;
		Token command = ctx.read_token(m);
		if (command == token("doif")) {
			level += 1;
		} else if (command == token("endi")) {
			level -= 1;
			if (level == 0) {
				ctx.read_command_separator(m);
				if (ctx.debug) {
					printf("ELSE jumped to %i\n", m.ip);
				}
				break;
			}
		} else {
			m.ip -= 3;
		}
	}
}

void Command_POSE(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t pose = ctx.read_int(m);

	auto* renderable = ctx.get_targ_part(m);
	renderable->clear_animation();
	renderable->sprite_index = pose;

	ctx.read_command_separator(m);
}

void Command_INST(MacroContext& ctx, Macro& m) {
	m.inst = true;
	ctx.read_command_separator(m);
}

void Command_UNTL(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	bool cond = ctx.read_condition(m);
	if (cond) {
		m.stack.pop_back();
		ctx.read_command_separator(m);
	} else {
		m.ip = m.stack.back();
	}
	//
	// throw Exception("UNTL is not implemented");
}

void Command_SETV(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t value = ctx.read_int(m);

	ctx.set_variable(m, varname, value);

	ctx.read_command_separator(m);
}

void Command_ADDV(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t right = ctx.read_int(m);

	int32_t left;
	if (!ctx.try_get_variable(m, varname, &left)) {
		throw Exception(fmt::format("Unknown variable {}", repr(varname)));
	}
	ctx.set_variable(m, varname, left + right);
	ctx.read_command_separator(m);
}

void Command_SUBV(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t subtrahend = ctx.read_int(m);

	int32_t minuend;
	if (!ctx.try_get_variable(m, varname, &minuend)) {
		throw Exception(fmt::format("Unknown variable {}", repr(varname)));
	}

	ctx.set_variable(m, varname, minuend - subtrahend);

	ctx.read_command_separator(m);
}

void MacroManager::tick() {
	// do something

	MacroContext ctx(*m_objects.get(), *m_renderables.get());
	// TODO: only initialize once
	ctx.command_funcs[token("addv")] = Command_ADDV;
	ctx.command_funcs[token("anim")] = Command_ANIM;
	ctx.command_funcs[token("doif")] = Command_DOIF;
	ctx.command_funcs[token("else")] = Command_ELSE;
	ctx.command_funcs[token("endi")] = Command_ENDI;
	ctx.command_funcs[token("ever")] = Command_EVER;
	ctx.command_funcs[token("inst")] = Command_INST;
	ctx.command_funcs[token("mesg")] = Command_MESG;
	ctx.command_funcs[token("mvby")] = Command_MVBY;
	ctx.command_funcs[token("over")] = Command_OVER;
	ctx.command_funcs[token("pose")] = Command_POSE;
	ctx.command_funcs[token("repe")] = Command_REPE;
	ctx.command_funcs[token("reps")] = Command_REPS;
	ctx.command_funcs[token("rndv")] = Command_RNDV;
	ctx.command_funcs[token("setv")] = Command_SETV;
	ctx.command_funcs[token("subv")] = Command_SUBV;
	ctx.command_funcs[token("sndc")] = Command_SNDC;
	ctx.command_funcs[token("snde")] = Command_SNDE;
	ctx.command_funcs[token("stim")] = Command_STIM;
	ctx.command_funcs[token("tick")] = Command_TICK;
	ctx.command_funcs[token("untl")] = Command_UNTL;
	ctx.command_funcs[token("wait")] = Command_WAIT;
	ctx.integerrv_funcs[token("posb")] = IntegerRV_POSB;
	ctx.integerrv_funcs[token("posl")] = IntegerRV_POSL;
	ctx.integerrv_funcs[token("xvec")] = IntegerRV_XVEC;
	ctx.agentrv_funcs[token("targ")] = AgentRV_TARG;
	ctx.agentrv_funcs[token("ownr")] = AgentRV_OWNR;
	ctx.agentrv_funcs[token("from")] = AgentRV_FROM;

	for (auto& m : m_pool) {
		ctx.debug = false;
		if (ctx.debug) {
			printf("DEBUG cls=(%i, %i, %i) uid=%i\n", ctx.get_ownr(m)->family, ctx.get_ownr(m)->genus, ctx.get_ownr(m)->species, m.ownr.id());
			printf("macro wait=%i - %s !!ip!! %s - subroutine %s %i\n", m.wait, m.script.substr(0, m.ip).c_str(), m.script.substr(m.ip).c_str(), m.subroutine_label.c_str(), m.subroutine_address);
			printf("stack ");
			// for (size_t i = 0;)
			for (auto s : m.stack) {
				printf("%i ", s);
			}
			printf("\n");
		}
		try {
			// we don't want to let inst run forever...
			for (size_t i = 0; i < (m.inst ? 100 : 1); ++i) {
				size_t original_ip = m.ip;
				try {
					Token command = ctx.read_token(m);
					if (command == token("endm") || command == token("stop")) {
						// remove from list, check selfdestruct
						m.ip = original_ip;
						continue;
					}
					auto it = ctx.command_funcs.find(command);
					if (it == ctx.command_funcs.end()) {
						throw Exception(fmt::format("Unknown command {}", repr(command)));
					}
					it->second(ctx, m);
				} catch (Exception& e) {
					m.ip = original_ip;
					throw;
				}
			}
		} catch (Exception& e) {
			printf("DEBUG cls=(%i, %i, %i) uid=%i\n", ctx.get_ownr(m)->family, ctx.get_ownr(m)->genus, ctx.get_ownr(m)->species, m.ownr.id());
			printf("macro %s !!ip!! %s\n", m.script.substr(0, m.ip).c_str(), m.script.substr(m.ip).c_str());
			fmt::print("error: {}\n", e.what());
		}
	}
}