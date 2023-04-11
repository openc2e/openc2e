#include "MessageManager.h"

#include "EngineContext.h"
#include "MacroManager.h"
#include "Object.h"
#include "ObjectHandle.h"
#include "ObjectManager.h"
#include "ObjectNames.h"
#include "Scriptorium.h"
#include "common/Exception.h"

void MessageManager::tick() {
	for (auto& m : m_immediate_message_queue) {
		auto* to = g_engine_context.objects->try_get(m.to);
		if (!to) {
			fmt::print("WARNING: discarding message with bad `to` object id\n");
		}

		switch (m.number) {
			case MESSAGE_ACTIVATE1: to->handle_mesg_activate1(m); break;
			case MESSAGE_ACTIVATE2: to->handle_mesg_activate2(m); break;
			case MESSAGE_DEACTIVATE: to->handle_mesg_deactivate(m); break;
			case MESSAGE_HIT: to->handle_mesg_hit(m); break;
			case MESSAGE_PICKUP: to->handle_mesg_pickup(m); break;
			case MESSAGE_DROP: to->handle_mesg_drop(m); break;
			default:
				throw Exception(fmt::format("Unknown message number {}", m.number));
		}
	}
	m_immediate_message_queue.clear();
}

void MessageManager::mesg_writ(ObjectHandle from_id, ObjectHandle to_id, MessageNumber message) {
	// TODO: implement delayed messages

	Message m;
	m.from = from_id;
	m.to = to_id;
	m.number = message;

	m_immediate_message_queue.push_back(m);
}
