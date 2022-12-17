

#include "TimerSystem.h"

#include "EngineContext.h"
#include "Object.h"
#include "ObjectManager.h"


void TimerSystem::tick() {
	for (auto& o : *g_engine_context.objects) {
		if (o->tick_value <= 0) {
			continue;
		}

		o->ticks_since_last_tick_event += 1;
		if (o->ticks_since_last_tick_event < o->tick_value) {
			continue;
		}

		o->ticks_since_last_tick_event = 0;

		if (!g_engine_context.events->queue_script(o, o, SCRIPT_TIMER)) {
			fmt::print("ERRO [TimerSystem] Disabling timer for {}\n", repr(o.get()));
			o->tick_value = 0;
		}
		// fmt::print("Fired timer script for {}, {}, {}\n", o->family, o->genus, o->species);
	}
}