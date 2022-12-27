#pragma once

#include "MacroManager.h"
#include "ObjectHandle.h"
#include "Scriptorium.h"
#include "common/PointerView.h"

enum MessageNumber {
	MESSAGE_ACTIVATE1 = 0,
	MESSAGE_ACTIVATE2 = 1,
	MESSAGE_DEACTIVATE = 2,
	MESSAGE_HIT = 3,
	MESSAGE_PICKUP = 4,
	MESSAGE_DROP = 5,
};

struct Message {
	ObjectHandle from;
	ObjectHandle to;
	MessageNumber number;
};

class EventManager {
  public:
	EventManager() {}
	void tick();

	bool queue_script(ObjectHandle from_id, ObjectHandle to_id, ScriptNumber eventno, bool override_existing = true);
	bool queue_script(PointerView<Object> from, PointerView<Object> to, ScriptNumber eventno, bool override_existing = true);
	void mesg_writ(ObjectHandle from_id, ObjectHandle to_id, MessageNumber message);

	std::vector<Message> m_immediate_message_queue;
};