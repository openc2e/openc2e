#pragma once

#include "EngineContext.h"

class TimerSystem {
  public:
	void tick() {
		for (auto& o : *g_engine_context.objects) {
			if (o->tick_value > 0) {
				o->ticks_since_last_tick_event += 1;
				if (o->ticks_since_last_tick_event >= o->tick_value) {
					o->ticks_since_last_tick_event = 0;

					g_engine_context.events->queue_script(o, o, SCRIPT_TIMER);
					fmt::print("Fired timer script for {}, {}, {}\n", o->family, o->genus, o->species);
				}
			}
		}
	}
};