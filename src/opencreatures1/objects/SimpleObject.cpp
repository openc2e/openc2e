#include "SimpleObject.h"

#include "MacroManager.h"
#include "SFCSerialization.h"
#include "fileformats/sfc/SimpleObject.h"

const DullPart* SimpleObject::get_part(int32_t partnum) const {
	if (partnum == 0) {
		return &part;
	}
	return nullptr;
}

void SimpleObject::handle_left_click(float, float) {
	// When an object is the subject of a left click event, we queue up a message
	// to ACTIVATE1, ACTIVATE2, or DEACTIVATE. But how do we know which message
	// to send?

	if (as_pointer_tool()) {
		throw Exception("handle_click erroneously called on PointerTool");
	}

	// SimpleObjects have their click behavior controlled by BHVR - we index into
	// their click BHVR array using their current ACTV (activation status), which
	// gives us the message to send.

	// fmt::print("Got a click, click bhvr {} {} {}, actv {}\n", as_simple_object()->click_bhvr[0], as_simple_object()->click_bhvr[1], as_simple_object()->click_bhvr[2], actv);

	int8_t click_message = as_simple_object()->click_bhvr[actv];
	// fmt::print("click message {}\n", click_message);
	if (click_message == -1) {
		// TODO: should we still make the mouse gesture?
		return;
	}
	g_engine_context.messages->mesg_writ(g_engine_context.pointer->m_pointer_tool, this->uid, MessageNumber(click_message));

	// let objects override the pointer script when they get clicked on. This seems to be only used by the Drum in the base world.
	if (!g_engine_context.macros->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, family, genus, species, SCRIPT_POINTER_ACTIVATE1)) {
		g_engine_context.macros->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, SCRIPT_POINTER_ACTIVATE1);
	}
}

void SimpleObject::serialize(SFCContext& ctx, sfc::SimpleObjectV1* simp) {
	if (ctx.is_storing()) {
		simp->part = sfc_dump_entity(part);
		static_cast<sfc::ObjectV1*>(simp)->gallery = simp->part->gallery;
		simp->z_order = z_order;
		simp->click_bhvr = click_bhvr;
		simp->touch_bhvr = touch_bhvr;
	} else {
		part = sfc_load_entity(simp->part.get());
		z_order = simp->z_order;
		click_bhvr = simp->click_bhvr;
		touch_bhvr = simp->touch_bhvr;
	}
	Object::serialize(ctx, simp);
}