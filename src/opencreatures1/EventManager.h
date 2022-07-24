#pragma once

#include "EngineContext.h"
#include "MacroManager.h"
#include "ObjectHandle.h"
#include "ObjectManager.h"
#include "Scriptorium.h"
#include "common/Exception.h"
#include "common/PointerView.h"

class EventManager {
  public:
	EventManager(EngineContext* ctx_)
		: ctx(ctx_) {}

	void queue_script(ObjectHandle from_id, ObjectHandle to_id, ScriptNumber eventno) {
		auto* to = ctx->objects->try_get<Object>(to_id);
		auto* from = ctx->objects->try_get<Object>(from_id);
		return queue_script(from, to, eventno);
	}

	void queue_script(PointerView<Object> from, PointerView<Object> to, ScriptNumber eventno, bool override_existing = false) {
		if (!to) {
			printf("WARNING: tried to run script %i on nonexistent object\n", eventno);
			return;
		}

		std::string script = ctx->scriptorium->get(to->family, to->genus, to->species, eventno);
		if (script.empty()) {
			printf("WARNING: tried to run nonexistent script %i %i %i %i\n", to->family, to->genus, to->species, eventno);
			return;
		}

		Macro m;
		m.script = script;
		m.ownr = to->uid;
		m.targ = m.ownr;
		m.from = from ? from->uid : ObjectHandle();

		if (override_existing) {
			throw_exception("override_existing not implemented");
		}
		for (auto& m : ctx->macros->m_pool) {
			if (m.ownr == to->uid) {
				return;
			}
		}
		ctx->macros->add(m);
	}

	void mesg_writ(ObjectHandle from_id, ObjectHandle to_id, MessageNumber message) {
		ScriptNumber eventno = [&] {
			switch (message) {
				case MESSAGE_ACTIVATE1: return SCRIPT_ACTIVATE1;
				case MESSAGE_ACTIVATE2: return SCRIPT_ACTIVATE2;
				case MESSAGE_DEACTIVATE: return SCRIPT_DEACTIVATE;
				case MESSAGE_HIT: return SCRIPT_HIT;
				case MESSAGE_PICKUP: return SCRIPT_PICKUP;
				case MESSAGE_DROP: return SCRIPT_DROP;
			}
			throw Exception(fmt::format("Unknown message number {}", message));
		}();

		queue_script(from_id, to_id, eventno);
	}

	EngineContext* ctx;
};