#include "MacroCommands.h"

#include "C1Sound.h"
#include "C1SoundManager.h"
#include "EventManager.h"
#include "Object.h"
#include "ObjectManager.h"
#include "common/Random.h"

void Command_ADDV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
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

void Command_ANIM(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	std::string anim_string = ctx.read_bracket_string(m);
	auto* r = ctx.get_targ_part(m);
	r->has_animation = true;
	r->animation_frame = 0;
	r->animation_string = anim_string;

	ctx.read_command_separator(m);
}


void Command_DOIF(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	bool cond = ctx.read_condition(m);
	ctx.instructions_left_this_tick++;
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
		if (command == Token("doif")) {
			level += 1;
		} else if (command == Token("else") && level == 1) {
			// cool, skip to this else
			ctx.read_command_separator(m);
			return;
		} else if (command == Token("endi")) {
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

void Command_DPAS(MacroContext& ctx, Macro& m) {
	printf("WARNING: DPAS not implemented\n");
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);
}

void Command_ELSE(MacroContext& ctx, Macro& m) {
	// TODO: hacky and probably wrong! get a better parser
	ctx.instructions_left_this_tick++;
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
		if (command == Token("doif")) {
			level += 1;
		} else if (command == Token("endi")) {
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

void Command_ENDI(MacroContext& ctx, Macro& m) {
	// I think we just move on?
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);
}

void Command_EVER(MacroContext& ctx, Macro& m) {
	// at the end of the loop, go back to start
	m.ip = numeric_cast<uint32_t>(m.stack.back());
	ctx.instructions_left_this_tick++;
}

void Command_FADE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	printf("WARNING: FADE not implemented\n");
	ctx.read_command_separator(m);
}

void Command_GPAS(MacroContext& ctx, Macro& m) {
	printf("WARNING: GPAS not implemented\n");
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);
}

void Command_INST(MacroContext& ctx, Macro& m) {
	m.inst = true;
	ctx.read_command_separator(m);
}

void Command_LOOP(MacroContext& ctx, Macro& m) {
	ctx.read_command_separator(m);
	m.stack.push_back(numeric_cast<int32_t>(m.ip));
	ctx.instructions_left_this_tick++;
}

void Command_MESG(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token subcommand = ctx.read_token(m);
	if (subcommand == Token("writ")) {
		ctx.read_arg_separator(m);
		ObjectHandle to = ctx.read_object(m);
		ctx.read_arg_separator(m);
		int32_t message = ctx.read_int(m);
		ctx.read_command_separator(m);

		ctx.events->mesg_writ(m.ownr, to, MessageNumber(message));

	} else {
		throw Exception(fmt::format("Unknown command 'mesg {}'", repr(subcommand)));
	}
}

void Command_MVBY(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t xdiff = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t ydiff = ctx.read_int(m);

	auto* obj = ctx.get_targ(m);

	move_object_by(obj, ctx.renderables, xdiff, ydiff);

	if (ctx.debug) {
		printf("did a mvby  x=%i y=%i cls=(%i, %i, %i) spr=%s!\n",
			xdiff, ydiff, ctx.get_targ(m)->family, ctx.get_targ(m)->genus, ctx.get_targ(m)->species, ctx.get_targ_part(m)->sprite.getName().c_str());
	}

	ctx.read_command_separator(m);
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

void Command_PART(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	int32_t part = ctx.read_int(m);
	ctx.read_command_separator(m);

	m.part = part;
}

void Command_POSE(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t pose = ctx.read_int(m);

	auto* renderable = ctx.get_targ_part(m);
	renderable->clear_animation();
	renderable->sprite_index = pose;

	ctx.read_command_separator(m);
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
		m.ip = numeric_cast<uint32_t>(loop_start);
	} else {
		m.stack.pop_back(); // iterations
		m.stack.pop_back(); // loop start
		if (ctx.debug) {
			printf("Done with REPE!\n");
		}
		ctx.read_command_separator(m);
	}
	ctx.instructions_left_this_tick++;
}

void Command_REPS(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t iterations = ctx.read_int(m);
	if (iterations <= 0) {
		throw Exception(fmt::format("REPS called with bad value: {}", iterations));
	}
	ctx.read_command_separator(m);
	m.stack.push_back(numeric_cast<int32_t>(m.ip));
	m.stack.push_back(iterations);
	ctx.instructions_left_this_tick++;
}

void Command_RNDV(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t low = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t high = ctx.read_int(m);

	int32_t generated = rand_int32(low, high);
	if (ctx.debug) {
		printf("RNDV info %s %i %i\n", repr(varname).c_str(), low, high);
		printf("RNDV generated %i\n", generated);
	}
	ctx.set_variable(m, varname, generated);

	ctx.read_command_separator(m);

	ctx.instructions_left_this_tick++;
}

void Command_SETV(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t value = ctx.read_int(m);

	ctx.set_variable(m, varname, value);

	ctx.read_command_separator(m);
	ctx.instructions_left_this_tick++;
}

void Command_SNDC(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	std::string sound_name = ctx.read_filename_token(m);

	auto* targ = ctx.get_targ(m);

	// TODO: what if it already has a controlled sound?
	if (targ->current_sound) {
		fmt::print("WARNING: Object already has a controlled sound: {}\n", repr(targ));
		ctx.read_command_separator(m);
		return;
	}

	C1Sound sound = ctx.sounds->play_sound(sound_name);
	auto bbox = get_object_bbox(targ, ctx.renderables);
	sound.set_position(bbox.left, bbox.top, bbox.width(), bbox.height());
	targ->current_sound = sound;

	ctx.read_command_separator(m);
}

void Command_SNDE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	std::string sound_name = ctx.read_filename_token(m);

	C1Sound sound = ctx.sounds->play_sound(sound_name);
	auto bbox = get_object_bbox(ctx.get_targ(m), ctx.renderables);
	sound.set_position(bbox.left, bbox.top, bbox.width(), bbox.height());

	ctx.read_command_separator(m);
}

void Command_SNDL(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	std::string sound_name = ctx.read_filename_token(m);

	auto* targ = ctx.get_targ(m);

	// TODO: what if it already has a controlled sound?
	if (targ->current_sound) {
		fmt::print("WARNING: Object already has a controlled sound: {}\n", repr(targ));
		ctx.read_command_separator(m);
		return;
	}

	C1Sound sound = ctx.sounds->play_sound(sound_name, true);
	auto bbox = get_object_bbox(targ, ctx.renderables);
	sound.set_position(bbox.left, bbox.top, bbox.width(), bbox.height());
	targ->current_sound = sound;

	ctx.read_command_separator(m);
}

void Command_STIM(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token subcommand = ctx.read_token(m);
	if (subcommand == Token("sign")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);

		printf("WARNING: STIM SIGN not implemented\n");

	} else if (subcommand == Token("writ")) {
		ctx.read_arg_separator(m);
		ctx.read_object(m);
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);

		printf("WARNING: STIM WRIT not implemented\n");

	} else if (subcommand == Token("shou")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);

		printf("WARNING: STIM SHOU not implemented\n");

	} else {
		throw Exception(fmt::format("Unknown command 'stim {}'", repr(subcommand)));
	}
}

void Command_STPC(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	auto* targ = ctx.get_targ(m);
	if (targ->current_sound) {
		targ->current_sound.stop();
	}
	ctx.read_command_separator(m);
}

void Command_SUBV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
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


void Command_TICK(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	int32_t tick_value = ctx.read_int(m);
	ctx.get_targ(m)->tick_value = tick_value;
	// TODO: should this be reset?
	ctx.get_targ(m)->ticks_since_last_tick_event = 0;

	ctx.read_command_separator(m);
}

void Command_WAIT(MacroContext& ctx, Macro& m) {
	auto old_ip = m.ip - 4;

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

void Command_UNTL(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	bool cond = ctx.read_condition(m);
	if (cond) {
		m.stack.pop_back();
		ctx.read_command_separator(m);
	} else {
		m.ip = numeric_cast<uint32_t>(m.stack.back());
	}
	ctx.instructions_left_this_tick++;
}

ObjectHandle AgentRV_FROM(MacroContext&, Macro& macro) {
	return macro.from;
}

ObjectHandle AgentRV_OWNR(MacroContext&, Macro& macro) {
	return macro.ownr;
}

ObjectHandle AgentRV_TARG(MacroContext&, Macro& macro) {
	return macro.targ;
}

int32_t IntegerRV_ACTV(MacroContext& ctx, Macro& m) {
	auto* targ = ctx.get_targ(m);
	return static_cast<int32_t>(targ->actv);
}

int32_t IntegerRV_POSB(MacroContext& ctx, Macro& m) {
	auto bbox = get_object_bbox(ctx.get_targ(m), ctx.renderables);
	return bbox.bottom;
}

int32_t IntegerRV_POSL(MacroContext& ctx, Macro& m) {
	auto bbox = get_object_bbox(ctx.get_targ(m), ctx.renderables);
	return bbox.left;
}

int32_t IntegerRV_XVEC(MacroContext& ctx, Macro& m) {
	Object* o = ctx.get_targ(m);
	auto* veh = dynamic_cast<Vehicle*>(o);
	if (!veh) {
		throw_exception("Called XVEC on non-vehicle object: {}", repr(*o));
	}

	return veh->xvel.raw();
}

void LValue_ACTV(const MacroContext& ctx, const Macro& m, int32_t value) {
	auto new_actv = ActiveFlag(value);
	Object* ownr = ctx.get_ownr(m);
	Object* targ = ctx.get_targ(m);

	if (new_actv == targ->actv) {
		return;
	}

	printf("setting active state %i!\n", new_actv);

	switch (new_actv) {
		case ACTV_INACTIVE:
			ctx.events->queue_script(ownr, targ, SCRIPT_DEACTIVATE);
			targ->actv = ACTV_INACTIVE;
			return;
		case ACTV_ACTIVE1:
			ctx.events->queue_script(ownr, targ, SCRIPT_ACTIVATE1);
			targ->actv = ACTV_ACTIVE1;
			return;
		case ACTV_ACTIVE2:
			ctx.events->queue_script(ownr, targ, SCRIPT_ACTIVATE2);
			targ->actv = ACTV_ACTIVE2;
			return;
	}

	throw_exception("Unknown active state {}", new_actv);
}

void LValue_XVEC(const MacroContext& ctx, const Macro& m, int32_t value) {
	Object* o = ctx.get_targ(m);
	auto* veh = dynamic_cast<Vehicle*>(o);
	if (!veh) {
		throw_exception("Called XVEC on non-vehicle object: {}", repr(*o));
	}
	veh->xvel = fixed24_8_t::from_raw(value);
}

void LValue_YVEC(const MacroContext& ctx, const Macro& m, int32_t value) {
	Object* o = ctx.get_targ(m);
	auto* veh = dynamic_cast<Vehicle*>(o);
	if (!veh) {
		throw_exception("Called YVEC on non-vehicle object: {}", repr(*o));
	}
	veh->yvel = fixed24_8_t::from_raw(value);
}

void MacroCommands::install_math_commands(MacroContext& ctx) {
	ctx.command_funcs[Token("addv")] = Command_ADDV;
	ctx.command_funcs[Token("subv")] = Command_SUBV;
	// TODO: mulv divv modv negv andv orrv
	ctx.command_funcs[Token("rndv")] = Command_RNDV;
}

void MacroCommands::install_default_commands(MacroContext& ctx) {
	install_math_commands(ctx);

	ctx.command_funcs[Token("anim")] = Command_ANIM;
	ctx.command_funcs[Token("dpas")] = Command_DPAS;
	ctx.command_funcs[Token("doif")] = Command_DOIF;
	ctx.command_funcs[Token("else")] = Command_ELSE;
	ctx.command_funcs[Token("endi")] = Command_ENDI;
	ctx.command_funcs[Token("ever")] = Command_EVER;
	ctx.command_funcs[Token("fade")] = Command_FADE;
	ctx.command_funcs[Token("gpas")] = Command_GPAS;
	ctx.command_funcs[Token("inst")] = Command_INST;
	ctx.command_funcs[Token("loop")] = Command_LOOP;
	ctx.command_funcs[Token("mesg")] = Command_MESG;
	ctx.command_funcs[Token("mvby")] = Command_MVBY;
	ctx.command_funcs[Token("over")] = Command_OVER;
	ctx.command_funcs[Token("part")] = Command_PART;
	ctx.command_funcs[Token("pose")] = Command_POSE;
	ctx.command_funcs[Token("repe")] = Command_REPE;
	ctx.command_funcs[Token("reps")] = Command_REPS;
	ctx.command_funcs[Token("setv")] = Command_SETV;
	ctx.command_funcs[Token("sndc")] = Command_SNDC;
	ctx.command_funcs[Token("snde")] = Command_SNDE;
	ctx.command_funcs[Token("sndl")] = Command_SNDL;
	ctx.command_funcs[Token("stim")] = Command_STIM;
	ctx.command_funcs[Token("stpc")] = Command_STPC;
	ctx.command_funcs[Token("tick")] = Command_TICK;
	ctx.command_funcs[Token("untl")] = Command_UNTL;
	ctx.command_funcs[Token("wait")] = Command_WAIT;

	ctx.agentrv_funcs[Token("from")] = AgentRV_FROM;
	ctx.agentrv_funcs[Token("ownr")] = AgentRV_OWNR;
	ctx.agentrv_funcs[Token("targ")] = AgentRV_TARG;

	ctx.integerrv_funcs[Token("actv")] = IntegerRV_ACTV;
	ctx.integerrv_funcs[Token("posb")] = IntegerRV_POSB;
	ctx.integerrv_funcs[Token("posl")] = IntegerRV_POSL;
	ctx.integerrv_funcs[Token("xvec")] = IntegerRV_XVEC;

	ctx.lvalue_funcs[Token("actv")] = LValue_ACTV;
	ctx.lvalue_funcs[Token("xvec")] = LValue_XVEC;
	ctx.lvalue_funcs[Token("yvec")] = LValue_YVEC;
}