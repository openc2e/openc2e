#include "EventManager.h"

#include "EngineContext.h"
#include "MacroManager.h"
#include "Object.h"
#include "ObjectHandle.h"
#include "ObjectManager.h"
#include "ObjectNames.h"
#include "Scriptorium.h"
#include "common/Exception.h"
#include "common/PointerView.h"

void EventManager::tick() {
	for (auto& m : m_immediate_message_queue) {
		switch (m.number) {
			case MESSAGE_ACTIVATE1: m.to->handle_mesg_activate1(m); break;
			case MESSAGE_ACTIVATE2: m.to->handle_mesg_activate2(m); break;
			case MESSAGE_DEACTIVATE: m.to->handle_mesg_deactivate(m); break;
			case MESSAGE_HIT: m.to->handle_mesg_hit(m); break;
			case MESSAGE_PICKUP: m.to->handle_mesg_pickup(m); break;
			case MESSAGE_DROP: m.to->handle_mesg_drop(m); break;
			default:
				throw Exception(fmt::format("Unknown message number {}", m.number));
		}
	}
	m_immediate_message_queue.clear();
}

void EventManager::queue_script(ObjectHandle from_id, ObjectHandle to_id, ScriptNumber eventno, bool override_existing) {
	auto* to = g_engine_context.objects->try_get(to_id);
	auto* from = g_engine_context.objects->try_get(from_id);
	return queue_script(from, to, eventno, override_existing);
}

std::string format_script(ScriptNumber eventno) {
	return fmt::format("{}", scriptnumber_to_string(eventno),
		eventno);
}

void EventManager::queue_script(PointerView<Object> from, PointerView<Object> to, ScriptNumber eventno, bool override_existing) {
	if (!to) {
		printf("WARNING: tried to run script %i on nonexistent object\n", eventno);
		return;
	}

	std::string script = g_engine_context.scriptorium->get(to->family, to->genus, to->species, eventno);
	if (script.empty()) {
		std::string script = g_engine_context.scriptorium->get(to->family, to->genus, 0, eventno);
	}
	if (script.empty()) {
		std::string script = g_engine_context.scriptorium->get(to->family, 0, 0, eventno);
	}
	if (script.empty()) {
		if (eventno == SCRIPT_INITIALIZE) {
			// skip, otherwise this raises a ton of (spurious?) warnings
			return;
		}
		fmt::print("WARN [EventManager] tried to run nonexistent script {} {}\n", repr(to), format_script(eventno));
		return;
	}

	Macro m;
	m.script = script;
	m.ownr = to->uid;
	m.targ = m.ownr;
	m.from = from ? from->uid : ObjectHandle();

	if (override_existing || eventno == SCRIPT_TIMER) {
		// TODO: will we break anything doing this right now? should we wait until end of frame?
		if (g_engine_context.macros->has_macro_owned_by(m.ownr)) {
			if (eventno == SCRIPT_TIMER) {
				// fmt::print("WARN [EventManager] Object {} {} {} skipping timer script because macro already exists\n", to->family, to->genus, to->species);
				return;
			}
			fmt::print("WARN [EventManager] {} replacing macro with {}, hope it doesn't break anything\n",
				repr(to), format_script(eventno));
			g_engine_context.macros->delete_macros_owned_by(m.ownr);
		}
	}
	g_engine_context.macros->add(m);
}

void EventManager::mesg_writ(ObjectHandle from_id, ObjectHandle to_id, MessageNumber message) {
	// TODO: implement delayed messages

	Message m;
	m.from = g_engine_context.objects->try_get(from_id);
	m.to = g_engine_context.objects->try_get(to_id);
	m.number = message;

	m_immediate_message_queue.push_back(m);
}
