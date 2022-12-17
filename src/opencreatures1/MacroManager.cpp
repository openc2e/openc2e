#include "MacroManager.h"

#include "Object.h"
#include "common/Exception.h"

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
		try {
			ctx.tick_macro(m);
		} catch (Exception& e) {
			fmt::print("error: {}\n", e.what());
			auto* owner = ctx.maybe_get_ownr(m);
			if (owner) {
				fmt::print("DEBUG cls=({}, {}, {}) uid={}\n", owner->family, owner->genus, owner->species, m.ownr);
			} else {
				fmt::print("DEBUG badownr uid={}\n", m.ownr);
			}
			fmt::print(
				"macro {} !!ip!! {}\n",
				m.script.substr(0, m.ip),
				m.ip <= m.script.size() ? m.script.substr(m.ip > m.script.size()) : "");
			fmt::print("\n");
		}
		if (m.destroy_as_soon_as_possible) {
			// in case we kicked off another script that wants to replace us, wait
			// until we're done running so we don't get weird errors
			m_pool.erase(m_pool.begin() + static_cast<long>(i - 1));
		}
	}
}
