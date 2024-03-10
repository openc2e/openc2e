#include "MacroManager.h"

#include "common/Exception.h"
#include "objects/Object.h"
#include "objects/ObjectManager.h"

std::string scriptnumber_to_string(ScriptNumber eventno) {
	struct scriptname {
		ScriptNumber eventno;
		const char* name;
	};

	static const scriptname scriptnames[] = {
		{SCRIPT_DEACTIVATE, "SCRIPT_DEACTIVATE"},
		{SCRIPT_ACTIVATE1, "SCRIPT_ACTIVATE1"},
		{SCRIPT_ACTIVATE2, "SCRIPT_ACTIVATE2"},
		{SCRIPT_HIT, "SCRIPT_HIT"},
		{SCRIPT_PICKUP, "SCRIPT_PICKUP"},
		{SCRIPT_DROP, "SCRIPT_DROP"},
		{SCRIPT_COLLISION, "SCRIPT_COLLISION"},
		{SCRIPT_INITIALIZE, "SCRIPT_INITIALIZE"},
		{SCRIPT_DEINITIALIZE, "SCRIPT_DEINITIALIZE"},
		{SCRIPT_TIMER, "SCRIPT_TIMER"},

		// creature
		{SCRIPT_EXTRA_QUIESCENT, "SCRIPT_EXTRA_QUIESCENT"},
		{SCRIPT_EXTRA_ACTIVATE1, "SCRIPT_EXTRA_ACTIVATE1"},
		{SCRIPT_EXTRA_ACTIVATE2, "SCRIPT_EXTRA_ACTIVATE2"},
		{SCRIPT_EXTRA_DEACTIVATE, "SCRIPT_EXTRA_DEACTIVATE"},
		{SCRIPT_EXTRA_SEEK, "SCRIPT_EXTRA_SEEK"},
		{SCRIPT_EXTRA_AVOID, "SCRIPT_EXTRA_AVOID"},
		{SCRIPT_EXTRA_PICKUP, "SCRIPT_EXTRA_PICKUP"},
		{SCRIPT_EXTRA_DROP, "SCRIPT_EXTRA_DROP"},
		{SCRIPT_EXTRA_NEED, "SCRIPT_EXTRA_NEED"},
		{SCRIPT_EXTRA_REST, "SCRIPT_EXTRA_REST"},
		{SCRIPT_EXTRA_WEST, "SCRIPT_EXTRA_WEST"},
		{SCRIPT_EXTRA_EAST, "SCRIPT_EXTRA_EAST"},
		{SCRIPT_EXTRA_UNDEFINED1, "SCRIPT_EXTRA_UNDEFINED1"},
		{SCRIPT_EXTRA_UNDEFINED2, "SCRIPT_EXTRA_UNDEFINED2"},
		{SCRIPT_EXTRA_UNDEFINED3, "SCRIPT_EXTRA_UNDEFINED3"},
		{SCRIPT_EXTRA_UNDEFINED4, "SCRIPT_EXTRA_UNDEFINED4"},

		// - introspective
		{SCRIPT_INTRO_QUIESCENT, "SCRIPT_INTRO_QUIESCENT"},
		{SCRIPT_INTRO_ACTIVATE1, "SCRIPT_INTRO_ACTIVATE1"},
		{SCRIPT_INTRO_ACTIVATE2, "SCRIPT_INTRO_ACTIVATE2"},
		{SCRIPT_INTRO_DEACTIVATE, "SCRIPT_INTRO_DEACTIVATE"},
		{SCRIPT_INTRO_SEEK, "SCRIPT_INTRO_SEEK"},
		{SCRIPT_INTRO_AVOID, "SCRIPT_INTRO_AVOID"},
		{SCRIPT_INTRO_PICKUP, "SCRIPT_INTRO_PICKUP"},
		{SCRIPT_INTRO_DROP, "SCRIPT_INTRO_DROP"},
		{SCRIPT_INTRO_NEED, "SCRIPT_INTRO_NEED"},
		{SCRIPT_INTRO_REST, "SCRIPT_INTRO_REST"},
		{SCRIPT_INTRO_WEST, "SCRIPT_INTRO_WEST"},
		{SCRIPT_INTRO_EAST, "SCRIPT_INTRO_EAST"},
		{SCRIPT_INTRO_UNDEFINED1, "SCRIPT_INTRO_UNDEFINED1"},
		{SCRIPT_INTRO_UNDEFINED2, "SCRIPT_INTRO_UNDEFINED2"},
		{SCRIPT_INTRO_UNDEFINED3, "SCRIPT_INTRO_UNDEFINED3"},
		{SCRIPT_INTRO_UNDEFINED4, "SCRIPT_INTRO_UNDEFINED4"},

		// pointer
		{SCRIPT_POINTER_ACTIVATE1, "SCRIPT_POINTER_ACTIVATE1"},
		{SCRIPT_POINTER_ACTIVATE2, "SCRIPT_POINTER_ACTIVATE2"},
		{SCRIPT_POINTER_DEACTIVATE, "SCRIPT_POINTER_DEACTIVATE"},
		{SCRIPT_POINTER_PICKUP, "SCRIPT_POINTER_PICKUP"},
		{SCRIPT_POINTER_DROP, "SCRIPT_POINTER_DROP"},

		// involuntary
		{SCRIPT_INVOLUNTARY0, "SCRIPT_INVOLUNTARY0"},
		{SCRIPT_INVOLUNTARY1, "SCRIPT_INVOLUNTARY1"},
		{SCRIPT_INVOLUNTARY2, "SCRIPT_INVOLUNTARY2"},
		{SCRIPT_INVOLUNTARY3, "SCRIPT_INVOLUNTARY3"},
		{SCRIPT_INVOLUNTARY4, "SCRIPT_INVOLUNTARY4"},
		{SCRIPT_INVOLUNTARY5, "SCRIPT_INVOLUNTARY5"},
		{SCRIPT_INVOLUNTARY6, "SCRIPT_INVOLUNTARY6"},
		{SCRIPT_INVOLUNTARY7, "SCRIPT_INVOLUNTARY7"},
		{SCRIPT_DIE, "SCRIPT_DIE"},
	};

	for (auto& s : scriptnames) {
		if (s.eventno == eventno) {
			return s.name;
		}
	}
	return "";
}

bool MacroManager::queue_script(ObjectHandle from_id, ObjectHandle to_id, ScriptNumber eventno, bool override_existing) {
	auto* to = g_engine_context.objects->try_get(to_id);
	auto* from = g_engine_context.objects->try_get(from_id);
	return queue_script(from, to, eventno, override_existing);
}

bool MacroManager::queue_script(ObjectHandle from_id, ObjectHandle to_id, uint8_t family, uint8_t genus, uint8_t species, ScriptNumber eventno, bool override_existing) {
	auto* to = g_engine_context.objects->try_get(to_id);
	auto* from = g_engine_context.objects->try_get(from_id);
	return queue_script(from, to, family, genus, species, eventno, override_existing);
}

static std::string format_script(ScriptNumber eventno) {
	return fmt::format("{}", scriptnumber_to_string(eventno), eventno);
}

bool MacroManager::queue_script(Object* from, Object* to, ScriptNumber eventno, bool override_existing) {
	// most of the time, we just want an object to run its own script
	if (!to) {
		printf("WARNING: tried to run script %i on nonexistent object\n", eventno);
		return false;
	}
	return queue_script(from, to, to->family, to->genus, to->species, eventno, override_existing);
}

bool MacroManager::queue_script(Object* from, Object* to, uint8_t family, uint8_t genus, uint8_t species, ScriptNumber eventno, bool override_existing) {
	// occasionally, we want an object to run a script from _another_ classifier — e.g. objects can override pointer animations

	if (!to) {
		printf("WARNING: tried to run script %i on nonexistent object\n", eventno);
		return false;
	}

	std::string script = g_engine_context.scriptorium->get(family, genus, species, eventno);
	if (script.empty()) {
		std::string script = g_engine_context.scriptorium->get(family, genus, 0, eventno);
	}
	if (script.empty()) {
		std::string script = g_engine_context.scriptorium->get(family, 0, 0, eventno);
	}
	if (script.empty()) {
		if (eventno == SCRIPT_INITIALIZE) {
			// skip, otherwise this raises a ton of (spurious?) warnings
			return false;
		}
		if (!(family == to->family && genus == to->genus && species == to->species)) {
			// skip, otherwise this raises warnings when the pointer does something (and we'll handle the return result anyways)
			return false;
		}
		fmt::print("WARN [MacroManager] tried to run nonexistent script {} {}\n", repr(to), format_script(eventno));
		return false;
	}

	Macro m;
	m.script = script;
	m.ownr = to->uid;
	m.targ = m.ownr;
	m.from = from ? from->uid : ObjectHandle();

	if (override_existing || eventno == SCRIPT_TIMER) {
		// TODO: will we break anything doing this right now? should we wait until end of frame?
		if (has_macro_owned_by(m.ownr)) {
			if (eventno == SCRIPT_TIMER) {
				// fmt::print("WARN [MacroManager] Object {} {} {} skipping timer script because macro already exists\n", to->family, to->genus, to->species);
				return true;
			}
			// fmt::print("WARN [MacroManager] {} replacing macro with {}, hope it doesn't break anything\n", repr(to), format_script(eventno));
			delete_macros_owned_by(m.ownr);
		}
	}
	add(m);
	return true;
}

void MacroManager::add(Macro macro) {
	m_pool.push_back(macro);
}

bool MacroManager::has_macro_owned_by(ObjectHandle ownr) const {
	for (auto& m : m_pool) {
		if (m.ownr == ownr) {
			return true;
		}
	}
	return false;
}

void MacroManager::delete_macros_owned_by(ObjectHandle ownr) {
	for (auto& m : m_pool) {
		if (m.ownr == ownr) {
			m.destroy_as_soon_as_possible = true;
		}
	}
}

void MacroManager::tick() {
	// do something

	// iterate in reverse, macros might be added/destroyed during iteration
	for (size_t i = m_pool.size(); i > 0; --i) {
		Macro& m = m_pool[i - 1];
		if (m.destroy_as_soon_as_possible) {
			m_pool.erase(m_pool.begin() + static_cast<long>(i - 1));
			continue;
		}
		ctx.tick_macro(m, true);
		if (m.destroy_as_soon_as_possible) {
			// in case we kicked off another script that wants to replace us, wait
			// until we're done running so we don't get weird errors
			m_pool.erase(m_pool.begin() + static_cast<long>(i - 1));
		}
	}
}
