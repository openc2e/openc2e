#pragma once

#include "ObjectHandle.h"
#include "Scriptorium.h"

/*

Messages are the primary way that agents interact with each other in Creatures.

For example, when a norn activate1s an agent, it actually sends it an activate1 message.
Some number of ticks later (?) the agent will receive and handle the message — potentially
running it's activate1 script, setting activation state, doing some special logic for its
object type (*cough*call buttons and lifts*cough*), or even stimming the creature with
disappointment.

*/

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

class ObjectMessageManager {
  public:
	ObjectMessageManager() {}
	void tick();

	void mesg_writ(ObjectHandle from_id, ObjectHandle to_id, MessageNumber message);

	std::vector<Message> m_immediate_message_queue;
};