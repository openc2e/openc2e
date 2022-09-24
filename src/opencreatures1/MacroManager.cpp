#include "MacroManager.h"

#include "Object.h"
#include "common/Exception.h"

void MacroManager::add(Macro macro) {
	m_pool.push_back(macro);
}

void MacroManager::tick() {
	// do something

	// iterate in reverse, macros might be added/destroyed during iteration
	for (size_t i = m_pool.size(); i > 0; --i) {
		Macro& m = m_pool[i - 1];
		try {
			ctx.tick_macro(m);
		} catch (Exception& e) {
			auto* owner = ctx.maybe_get_ownr(m);
			if (owner) {
				fmt::print("DEBUG cls=({}, {}, {}) uid={}\n", owner->family, owner->genus, owner->species, m.ownr);
			} else {
				fmt::print("DEBUG badownr uid={}\n", m.ownr);
			}
			fmt::print("macro {} !!ip!! {}\n", m.script.substr(0, m.ip), m.script.substr(m.ip));
			fmt::print("error: {}\n", e.what());
		}
	}
}
