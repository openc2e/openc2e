#include "MacroManager.h"

#include "Object.h"
#include "common/Exception.h"


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
