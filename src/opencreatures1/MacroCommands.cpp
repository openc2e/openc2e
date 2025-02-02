#include "MacroCommands.h"

#include "C1ControlledSound.h"
#include "C1SoundManager.h"
#include "Macro.h"
#include "MacroContext.h"
#include "MessageManager.h"
#include "common/Random.h"
#include "objects/Blackboard.h"
#include "objects/Object.h"
#include "objects/ObjectManager.h"
#include "objects/Vehicle.h"

void Command_ADDV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t right = ctx.read_int(m);

	int32_t left;
	if (!ctx.try_get_variable(m, varname, &left)) {
		throw Exception(fmt::format("Unknown variable {:?}", varname));
	}
	ctx.set_variable(m, varname, left + right);
	ctx.read_command_separator(m);
}

void Command_ANIM(MacroContext& ctx, Macro& m) {
	if (m.script[m.ip] == '[') {
		// The learning computer and cloud butterflies have scripts that go
		// `anim[0123]` with no whitespace separating the ANIM command and
		// the string argument. If we're followed directly by the string
		// bracket, skip trying to parse a separator.
	} else {
		ctx.read_arg_separator(m);
	}

	std::string anim_string = ctx.read_bracket_string(m);
	auto* r = ctx.get_targ_part(m);
	r->set_animation(0, anim_string);

	ctx.read_command_separator(m);
}

void Command_BASE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	int32_t new_base = ctx.read_int(m);

	auto* r = ctx.get_targ_part(m);
	// TODO: I think we both set the new base and reset the pose to 0?
	r->set_base(new_base);
	r->set_pose(0);

	ctx.read_command_separator(m);
}

void Command_BBDcolon(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);

	auto targ = ctx.get_targ(m);
	auto bbd = targ->as_blackboard();

	Token subcommand = ctx.read_token(m);
	ctx.read_arg_separator(m);

	if (subcommand == Token("word")) {
		int32_t index = ctx.read_int(m);
		ctx.read_arg_separator(m);
		int32_t id = ctx.read_int(m);
		ctx.read_arg_separator(m);
		Token word = ctx.read_token(m);
		fmt::print("WARNING: BBD: WORD {} {} {:?} not implemented\n", index, id, word);

	} else if (subcommand == Token("show")) {
		int32_t enable = ctx.read_int(m);
		if (enable == 1) {
			bbd->blackboard_show_word(targ->obv0);
		} else if (enable == 0) {
			bbd->blackboard_hide_word();
		} else {
			throw Exception(fmt::format("BBD: SHOW {} invalid argument", enable));
		}

	} else if (subcommand == Token("emit")) {
		int32_t volume = ctx.read_int(m);
		if (volume > 0) {
			bbd->blackboard_emit_earshot(targ->obv0);
		} else if (volume == 0) {
			bbd->blackboard_emit_eyesight(targ->obv0);
		} else {
			throw Exception(fmt::format("BBD: EMIT {} invalid argument", volume));
		}

	} else if (subcommand == Token("edit")) {
		int32_t enable = ctx.read_int(m);
		if (enable == 1) {
			bbd->blackboard_enable_edit();
		} else if (enable == 0) {
			bbd->blackboard_disable_edit();
		} else {
			throw Exception(fmt::format("BBD: EDIT {} invalid argument", enable));
		}

	} else {
		throw UnknownMacroCommand(fmt::format("unknown command bbd: {:?}", subcommand));
	}
	ctx.read_command_separator(m);
}

void Command_DIVV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t divisor = ctx.read_int(m);

	int32_t dividend;
	if (!ctx.try_get_variable(m, varname, &dividend)) {
		throw Exception(fmt::format("Unknown variable {:?}", varname));
	}

	ctx.set_variable(m, varname, dividend / divisor);

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

void Command_DONE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);
	printf("WARNING: DONE not implemented\n");
}

void Command_DPAS(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);
	ctx.get_targ(m)->vehicle_drop_passengers();
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

void Command_ENUM(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	int32_t family = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t genus = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t species = ctx.read_int(m);
	ctx.read_command_separator(m);

	if (family < 0 || family > 255 || genus < 0 || genus > 255 || species < 0 || species > 255) {
		throw Exception(fmt::format("Invalid arguments to ENUM {} {} {}", family, genus, species));
	}

	if (m.enum_result.size()) {
		throw Exception("Nested calls to ENUM are not implemented");
	}

	for (auto* obj : *g_engine_context.objects) {
		if ((obj->family == family || family == 0) && (obj->genus == genus || genus == 0) && (obj->species == species || species == 0)) {
			m.enum_result.push_back(obj->uid);
		}
	}

	// if we have results, then run the block
	if (m.enum_result.size()) {
		m.stack.push_back(static_cast<int32_t>(m.ip)); // where to jump back to
		m.targ = m.enum_result.back();
		m.enum_result.pop_back();
		return;
	}

	// otherwise, if no objects, go find the next NEXT
	// TODO: hacky and probably wrong! get a better parser
	size_t level = 1;
	while (true) {
		if (m.script[m.ip] == '\0') {
			throw Exception("Unexpected eoi while looking for NEXT");
		}
		if (!(m.script[m.ip] == ',' || m.script[m.ip] == ' ')) {
			m.ip++;
			continue;
		}
		m.ip++;
		Token command = ctx.read_token(m);
		if (command == Token("enum")) {
			level += 1;
		} else if (command == Token("next")) {
			level -= 1;
			if (level == 0) {
				// cool, skip past this NEXT
				ctx.read_command_separator(m);
				return;
			}
		} else {
			m.ip -= 3;
		}
	}
}

void Command_EVER(MacroContext& ctx, Macro& m) {
	// at the end of the loop, go back to start
	m.ip = numeric_cast<uint32_t>(m.stack.back());
	ctx.instructions_left_this_tick++;
}

void Command_FADE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);

	ctx.get_targ(m)->current_sound.fade_out();
}

void Command_GPAS(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);
	ctx.get_targ(m)->vehicle_grab_passengers();
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

		g_engine_context.messages->mesg_writ(m.ownr, to, MessageNumber(message));

	} else {
		throw Exception(fmt::format("Unknown command 'mesg {:?}'", subcommand));
	}
}

void Command_MULV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t multiplier = ctx.read_int(m);

	int32_t multiplicand;
	if (!ctx.try_get_variable(m, varname, &multiplicand)) {
		throw Exception(fmt::format("Unknown variable {:?}", varname));
	}

	ctx.set_variable(m, varname, multiplicand * multiplier);

	ctx.read_command_separator(m);
}

void Command_MVBY(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t xdiff = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t ydiff = ctx.read_int(m);

	auto* obj = ctx.get_targ(m);
	obj->add_position(xdiff, ydiff);

	if (ctx.debug) {
		fmt::print("did a mvby  x={} y={} cls=({}, {}, {}) spr={}!\n",
			xdiff, ydiff, ctx.get_targ(m)->family, ctx.get_targ(m)->genus,
			ctx.get_targ(m)->species, ctx.get_targ_part(m)->gallery().name);
	}

	ctx.read_command_separator(m);
}

void Command_MVTO(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	int32_t x = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t y = ctx.read_int(m);

	auto* obj = ctx.get_targ(m);

	obj->set_position(x, y);

	// if (ctx.debug) {
	// 	fmt::print("did a mvby  x={} y={} cls=({}, {}, {}) spr={}!\n",
	// 		xdiff, ydiff, ctx.get_targ(m)->family, ctx.get_targ(m)->genus, ctx.get_targ(m)->species, ctx.get_targ_part(m)->sprite.getName());
	// }

	ctx.read_command_separator(m);
}

void Command_NEGV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);

	int32_t value;
	if (!ctx.try_get_variable(m, varname, &value)) {
		throw Exception(fmt::format("Unknown variable {:?}", varname));
	}

	ctx.set_variable(m, varname, -value);

	ctx.read_command_separator(m);
}

void Command_NEXT(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_command_separator(m);

	while (m.enum_result.size()) {
		ObjectHandle obj = m.enum_result.back();
		m.enum_result.pop_back();
		if (g_engine_context.objects->try_get(obj)) {
			m.targ = obj;
			m.ip = static_cast<uint32_t>(m.stack.back());
			return;
		}
	}

	// done with the ENUM, remove the block ip from the stack
	m.stack.pop_back();
}

void Command_OVER(MacroContext& ctx, Macro& m) {
	if (ctx.get_targ_part(m)->has_animation()) {
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
	ctx.read_command_separator(m);

	if (ctx.get_targ(m)->as_creature()) {
		printf("WARN called POSE on Creature, not implemented\n");
		return;
	}

	auto* part = ctx.get_targ_part(m);
	part->clear_animation();
	part->set_pose(pose);
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
		fmt::print("RNDV info {:?} {} {}\n", varname, low, high);
		fmt::print("RNDV generated {}\n", generated);
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
	ctx.read_command_separator(m);

	auto* targ = ctx.get_targ(m);
	C1ControlledSound sound = g_engine_context.sounds->play_controlled_sound(sound_name, targ->get_bbox());
	if (!sound) {
		return;
	}
	targ->current_sound.fade_out();
	targ->current_sound = std::move(sound);
}

void Command_SNDE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	std::string sound_name = ctx.read_filename_token(m);
	ctx.read_command_separator(m);

	auto* targ = ctx.get_targ(m);
	// not controlled, don't override existing sounds or move with object
	g_engine_context.sounds->play_uncontrolled_sound(sound_name, targ->get_bbox());
}

void Command_SNDL(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	std::string sound_name = ctx.read_filename_token(m);
	ctx.read_command_separator(m);

	auto* targ = ctx.get_targ(m);
	C1ControlledSound sound = g_engine_context.sounds->play_controlled_sound(sound_name, targ->get_bbox(), true);
	if (!sound) {
		return;
	}

	targ->current_sound.fade_out();
	targ->current_sound = std::move(sound);
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
		ctx.get_targ(m)->stim_sign();

	} else if (subcommand == Token("writ")) {
		ctx.read_arg_separator(m);
		auto* subject = g_engine_context.objects->try_get(ctx.read_object(m));
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);
		if (subject) {
			subject->creature_stim_writ();
		}

	} else if (subcommand == Token("shou")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);
		ctx.get_targ(m)->stim_shou();

	} else if (subcommand == Token("tact")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);

		printf("WARNING: STIM TACT not implemented\n");

	} else if (subcommand == Token("from")) {
		for (int i = 0; i < 12; ++i) {
			ctx.read_arg_separator(m);
			ctx.read_int(m);
		}
		ctx.read_command_separator(m);

		// TODO: The Creatures 1 learning computer calls STIM FROM. It seems to
		// be ignored.
		printf("WARNING: Treating invalid STIM FROM command as a no-op\n");

	} else {
		throw Exception(fmt::format("Unknown command 'stim {:?}'", subcommand));
	}
}

void Command_STMpound(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token subcommand = ctx.read_token(m);
	if (subcommand == Token("sign")) {
		ctx.read_arg_separator(m);
		ctx.read_int(m);
		ctx.read_command_separator(m);
		printf("WARNING: STM# SIGN not implemented\n");

	} else if (subcommand == Token("writ")) {
		ctx.read_arg_separator(m);
		ctx.read_object(m);
		ctx.read_arg_separator(m);
		ctx.read_int(m);
		ctx.read_command_separator(m);
		printf("WARNING: STM# WRIT not implemented\n");

	} else if (subcommand == Token("tact")) {
		ctx.read_arg_separator(m);
		ctx.read_int(m);
		ctx.read_command_separator(m);

		printf("WARNING: STM# TACT not implemented\n");

	} else if (subcommand == Token("shou")) {
		ctx.read_arg_separator(m);
		ctx.read_int(m);
		ctx.read_command_separator(m);

		printf("WARNING: STM# SHOU not implemented\n");

	} else {
		throw Exception(fmt::format("Unknown command 'STM# {:?}'", subcommand));
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

void Command_TARG(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	ObjectHandle new_targ = ctx.read_object(m);
	ctx.set_targ(m, new_targ);
	ctx.read_command_separator(m);
}

void Command_TELE(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	int32_t x = ctx.read_int(m);
	ctx.read_arg_separator(m);
	int32_t y = ctx.read_int(m);
	ctx.read_command_separator(m);

	printf("WARNING: TELE %i %i not implemented\n", x, y);
}

void Command_SUBV(MacroContext& ctx, Macro& m) {
	ctx.instructions_left_this_tick++;
	ctx.read_arg_separator(m);
	Token varname = ctx.read_token(m);
	ctx.read_arg_separator(m);
	int32_t subtrahend = ctx.read_int(m);

	int32_t minuend;
	if (!ctx.try_get_variable(m, varname, &minuend)) {
		throw Exception(fmt::format("Unknown variable {:?}", varname));
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

ObjectHandle AgentRV__IT_(MacroContext& ctx, Macro& m) {
	// TODO: is this correct for creatures?
	if (auto* ownr = ctx.maybe_get_ownr(m)) {
		if (ownr->as_creature()) {
			printf("WARN called _IT_ on Creature, not sure if this is implemented correctly\n");
		}
	}
	return m._it_;
}

ObjectHandle AgentRV_FROM(MacroContext&, Macro& macro) {
	return macro.from;
}

ObjectHandle AgentRV_OWNR(MacroContext&, Macro& macro) {
	return macro.ownr;
}

ObjectHandle AgentRV_PNTR(MacroContext&, Macro&) {
	return g_engine_context.pointer->m_pointer_tool;
}

ObjectHandle AgentRV_TARG(MacroContext&, Macro& macro) {
	return macro.targ;
}

int32_t IntegerRV_ACTV(MacroContext& ctx, Macro& m) {
	auto* targ = ctx.get_targ(m);
	return static_cast<int32_t>(targ->actv);
}

int32_t IntegerRV_HGHT(MacroContext& ctx, Macro& m) {
	return numeric_cast<int32_t>(ctx.get_targ(m)->get_bbox().height);
}

int32_t IntegerRV_LIMB(MacroContext& ctx, Macro& m) {
	auto* targ = ctx.get_targ(m);
	return targ->limit.bottom();
}

int32_t IntegerRV_LIML(MacroContext& ctx, Macro& m) {
	auto* targ = ctx.get_targ(m);
	return targ->limit.x;
}

int32_t IntegerRV_LIMR(MacroContext& ctx, Macro& m) {
	auto* targ = ctx.get_targ(m);
	return targ->limit.right();
}

int32_t IntegerRV_LIMT(MacroContext& ctx, Macro& m) {
	auto* targ = ctx.get_targ(m);
	return targ->limit.y;
}

int32_t IntegerRV_POSB(MacroContext& ctx, Macro& m) {
	return numeric_cast<int32_t>(ctx.get_targ(m)->get_bbox().bottom());
}

int32_t IntegerRV_POSE(MacroContext& ctx, Macro& m) {
	if (ctx.get_targ(m)->as_creature()) {
		printf("WARN called POSE on Creature, not implemented\n");
		return 0;
	}
	return ctx.get_targ_part(m)->pose();
}

int32_t IntegerRV_POSL(MacroContext& ctx, Macro& m) {
	return numeric_cast<int32_t>(ctx.get_targ(m)->get_bbox().x);
}

int32_t IntegerRV_POSR(MacroContext& ctx, Macro& m) {
	return numeric_cast<int32_t>(ctx.get_targ(m)->get_bbox().right());
}

int32_t IntegerRV_POST(MacroContext& ctx, Macro& m) {
	return numeric_cast<int32_t>(ctx.get_targ(m)->get_bbox().y);
}

int32_t IntegerRV_TOTL(MacroContext& ctx, Macro& m) {
	ctx.read_arg_separator(m);
	auto family = ctx.read_int(m);
	ctx.read_arg_separator(m);
	auto genus = ctx.read_int(m);
	ctx.read_arg_separator(m);
	auto species = ctx.read_int(m);

	return g_engine_context.objects->count_classifier(family, genus, species);
}

int32_t IntegerRV_WDTH(MacroContext& ctx, Macro& m) {
	return numeric_cast<int32_t>(ctx.get_targ(m)->get_bbox().width);
}

int32_t IntegerRV_XVEC(MacroContext& ctx, Macro& m) {
	Object* o = ctx.get_targ(m);
	auto* veh = o->as_vehicle();
	if (!veh) {
		throw_exception("Called XVEC on non-vehicle object: {:?}", *o);
	}

	return numeric_cast<int32_t>(veh->xvel * 256);
}

void LValue_ACTV(const MacroContext& ctx, const Macro& m, int32_t value) {
	auto new_actv = ActiveFlag(value);
	Object* targ = ctx.get_targ(m);

	switch (new_actv) {
		case ACTV_INACTIVE:
			targ->actv = ACTV_INACTIVE;
			return;
		case ACTV_ACTIVE1:
			targ->actv = ACTV_ACTIVE1;
			return;
		case ACTV_ACTIVE2:
			targ->actv = ACTV_ACTIVE2;
			return;
	}

	throw_exception("Unknown active state {}", new_actv);
}

void LValue_XVEC(const MacroContext& ctx, const Macro& m, int32_t value) {
	Object* o = ctx.get_targ(m);
	auto* veh = o->as_vehicle();
	if (!veh) {
		throw_exception("Called XVEC on non-vehicle object: {:?}", *o);
	}
	veh->xvel = value / 256.f;
}

void LValue_YVEC(const MacroContext& ctx, const Macro& m, int32_t value) {
	Object* o = ctx.get_targ(m);
	auto* veh = o->as_vehicle();
	if (!veh) {
		throw_exception("Called YVEC on non-vehicle object: {:?}", *o);
	}
	veh->yvel = value / 256.f;
}

void MacroCommands::install_math_commands(MacroContext& ctx) {
	ctx.command_funcs[Token("addv")] = Command_ADDV;
	ctx.command_funcs[Token("divv")] = Command_DIVV;
	ctx.command_funcs[Token("mulv")] = Command_MULV;
	ctx.command_funcs[Token("negv")] = Command_NEGV;
	ctx.command_funcs[Token("subv")] = Command_SUBV;
	// TODO: modv andv orrv
	ctx.command_funcs[Token("rndv")] = Command_RNDV;
}

void MacroCommands::install_default_commands(MacroContext& ctx) {
	install_math_commands(ctx);

	ctx.command_funcs[Token("anim")] = Command_ANIM;
	ctx.command_funcs[Token("base")] = Command_BASE;
	ctx.command_funcs[Token("bbd:")] = Command_BBDcolon;
	ctx.command_funcs[Token("dpas")] = Command_DPAS;
	ctx.command_funcs[Token("doif")] = Command_DOIF;
	ctx.command_funcs[Token("done")] = Command_DONE;
	ctx.command_funcs[Token("else")] = Command_ELSE;
	ctx.command_funcs[Token("endi")] = Command_ENDI;
	ctx.command_funcs[Token("enum")] = Command_ENUM;
	ctx.command_funcs[Token("ever")] = Command_EVER;
	ctx.command_funcs[Token("fade")] = Command_FADE;
	ctx.command_funcs[Token("gpas")] = Command_GPAS;
	ctx.command_funcs[Token("inst")] = Command_INST;
	ctx.command_funcs[Token("loop")] = Command_LOOP;
	ctx.command_funcs[Token("mesg")] = Command_MESG;
	ctx.command_funcs[Token("mvby")] = Command_MVBY;
	ctx.command_funcs[Token("mvto")] = Command_MVTO;
	ctx.command_funcs[Token("next")] = Command_NEXT;
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
	ctx.command_funcs[Token("stm#")] = Command_STMpound;
	ctx.command_funcs[Token("stpc")] = Command_STPC;
	ctx.command_funcs[Token("targ")] = Command_TARG;
	ctx.command_funcs[Token("tele")] = Command_TELE;
	ctx.command_funcs[Token("tick")] = Command_TICK;
	ctx.command_funcs[Token("untl")] = Command_UNTL;
	ctx.command_funcs[Token("wait")] = Command_WAIT;

	ctx.agentrv_funcs[Token("_it_")] = AgentRV__IT_;
	ctx.agentrv_funcs[Token("from")] = AgentRV_FROM;
	ctx.agentrv_funcs[Token("ownr")] = AgentRV_OWNR;
	ctx.agentrv_funcs[Token("pntr")] = AgentRV_PNTR;
	ctx.agentrv_funcs[Token("targ")] = AgentRV_TARG;

	ctx.integerrv_funcs[Token("actv")] = IntegerRV_ACTV;
	ctx.integerrv_funcs[Token("hght")] = IntegerRV_HGHT;
	ctx.integerrv_funcs[Token("limb")] = IntegerRV_LIMB;
	ctx.integerrv_funcs[Token("liml")] = IntegerRV_LIML;
	ctx.integerrv_funcs[Token("limr")] = IntegerRV_LIMR;
	ctx.integerrv_funcs[Token("limt")] = IntegerRV_LIMT;
	ctx.integerrv_funcs[Token("posb")] = IntegerRV_POSB;
	ctx.integerrv_funcs[Token("pose")] = IntegerRV_POSE;
	ctx.integerrv_funcs[Token("posl")] = IntegerRV_POSL;
	ctx.integerrv_funcs[Token("posr")] = IntegerRV_POSR;
	ctx.integerrv_funcs[Token("post")] = IntegerRV_POST;
	ctx.integerrv_funcs[Token("totl")] = IntegerRV_TOTL;
	ctx.integerrv_funcs[Token("wdth")] = IntegerRV_WDTH;
	ctx.integerrv_funcs[Token("xvec")] = IntegerRV_XVEC;

	ctx.lvalue_funcs[Token("actv")] = LValue_ACTV;
	ctx.lvalue_funcs[Token("xvec")] = LValue_XVEC;
	ctx.lvalue_funcs[Token("yvec")] = LValue_YVEC;
}