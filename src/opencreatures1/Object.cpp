#include "Object.h"

#include "ObjectManager.h"

void Object::handle_click() {
	if (simple_data) {
		if (pointer_data) {
			throw Exception("handle_click erroneously called on PointerTool");
		}

		fmt::print("Got a click, click bhvr {} {} {}, actv {}\n", simple_data->click_bhvr[0], simple_data->click_bhvr[1], simple_data->click_bhvr[2], actv);

		int8_t click_message = simple_data->click_bhvr[actv];
		fmt::print("click message {}\n", click_message);
		if (click_message == -1) {
			return;
		}
		g_engine_context.events->mesg_writ(g_engine_context.pointer->m_pointer_tool, this->uid, MessageNumber(click_message));

		// TODO: let objects override the script when they get clicked on
		g_engine_context.events->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, SCRIPT_POINTER_ACTIVATE1);
		return;
	}
	if (compound_data) {
		throw Exception("handle_click not implemented for CompoundObject");
	}
	throw Exception("handle_click not implemented");
}

void Object::handle_mesg_activate1(Message) {
	if (simple_data && !pointer_data) {
		// TODO: if from a creature, already in act1 or touch_bhvr doesn't allow act1, stim disappointment
		actv = ACTV_ACTIVE1;
		// TODO: from object?
		g_engine_context.events->queue_script(nullptr, this, SCRIPT_ACTIVATE1);
		return;
	}
	if (vehicle_data) {
		// TODO: if from a creature and already act1, or doesn't allow act1, stim disappointment
		actv = ACTV_ACTIVE1;
		move_object_to(this, get_renderable_for_part(0)->x.trunc(), get_renderable_for_part(0)->y.trunc());
		g_engine_context.events->queue_script(nullptr, this, SCRIPT_ACTIVATE1);
		return;
	}
	throw Exception(fmt::format("handle_mesg_activate1 not implemented on object {} {} {}", family, genus, species));
}
void Object::handle_mesg_activate2(Message) {
	// TODO
	// throw Exception("handle_mesg_activate2 not implemented");
}
void Object::handle_mesg_deactivate(Message) {
	if (simple_data && !pointer_data) {
		// TODO: if from a creature, already in deact or touch_bhvr doesn't allow deact, stim disappointment
		actv = ACTV_INACTIVE;
		// TODO: from object?
		g_engine_context.events->queue_script(nullptr, this, SCRIPT_DEACTIVATE);
		return;
	}
	if (vehicle_data) {
		// TODO: if from a creature and already inactive, or can't be deactivated, stim disappointment
		if (actv != ACTV_INACTIVE) {
			vehicle_data->xvel = 0;
			vehicle_data->yvel = 0;
			move_object_to(this, get_renderable_for_part(0)->x.trunc(), get_renderable_for_part(0)->y.trunc());
			actv = ACTV_INACTIVE;
			// TODO: from object?
			g_engine_context.events->queue_script(nullptr, this, SCRIPT_DEACTIVATE);
		}
		return;
	}
	throw Exception("handle_mesg_deactivate not implemented");
}
void Object::handle_mesg_hit(Message) {
	throw Exception("handle_mesg_hit not implemented");
}
void Object::handle_mesg_pickup(Message) {
	throw Exception("handle_mesg_pickup not implemented");
}
void Object::handle_mesg_drop(Message) {
	throw Exception("handle_mesg_drop not implemented");
}

Renderable* Object::get_renderable_for_part(int32_t partnum) {
	if (scenery_data) {
		if (partnum == 0) {
			return &scenery_data->part;
		}
	} else if (simple_data) {
		if (partnum == 0) {
			return &simple_data->part;
		}
	} else if (compound_data) {
		auto idx = numeric_cast<uint32_t>(partnum);
		if (idx >= compound_data->parts.size()) {
			return {};
		}
		return &compound_data->parts[idx].renderable;
	}
	return nullptr;
}