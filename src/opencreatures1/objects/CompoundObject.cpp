#include "CompoundObject.h"

#include "MacroManager.h"
#include "SFCSerialization.h"
#include "fileformats/sfc/CompoundObject.h"
#include "fileformats/sfc/Entity.h"

const DullPart* CompoundObject::get_part(int32_t partnum) const {
	if (partnum >= 0 && numeric_cast<size_t>(partnum) < parts.size()) {
		return &parts[numeric_cast<size_t>(partnum)];
	}
	return nullptr;
}

void CompoundObject::handle_left_click(float relx, float rely) {
	// When an object is the subject of a left click event, we queue up a message
	// to ACTIVATE1, ACTIVATE2, or DEACTIVATE. But how do we know which message
	// to send?

	// CompoundObjects are different from SimpleObjects. We check the clickable knobs
	// (out of the six total knobs, the first three are for creatures, the second
	// three are for the mouse) and their associated hotspots to see if any contain
	// the click location.

	// printf("handle_left_click %i %i\n", relx, rely);

	for (size_t i = 3; i < 6; ++i) {
		int32_t hotspot_idx = functions_to_hotspots[i];
		if (hotspot_idx == -1 || hotspot_idx < 0) {
			// knob doesn't have hotspot attached
			continue;
		}
		auto hotspot = hotspots[numeric_cast<size_t>(hotspot_idx)];
		// TODO: check for bad hotspots?

		if (hotspot.has_point(relx, rely)) {
			// Found a clickable knob whose hotspot contains this click!
			g_engine_context.messages->mesg_writ(g_engine_context.pointer->m_pointer_tool, this->uid, MessageNumber(i - 3));

			// let objects override the pointer script when they get clicked on. This seems to be only used by the Drum (a SimpleObject) in the base world.
			if (!g_engine_context.macros->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, family, genus, species, SCRIPT_POINTER_ACTIVATE1)) {
				g_engine_context.macros->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, SCRIPT_POINTER_ACTIVATE1);
			}
			return;
		}
	}
	// TODO: what if there are no knobs/hotspots?
}

void CompoundObject::serialize(SFCContext& ctx, sfc::CompoundObjectV1* comp) {
	if (ctx.is_storing()) {
		for (auto& part : parts) {
			sfc::CompoundPartV1 sfcpart;
			sfcpart.entity = sfc_dump_entity(part, comp->gallery);
			if (!comp->gallery) {
				comp->gallery = sfcpart.entity->gallery;
			}
			sfcpart.relx = part.relx;
			sfcpart.rely = part.rely;
			comp->parts.push_back(sfcpart);
		}
		for (size_t i = 0; i < comp->hotspots.size(); ++i) {
			comp->hotspots[i] = hotspots[i];
		}
		comp->functions_to_hotspots = functions_to_hotspots;
	} else {
		for (auto& cp : comp->parts) {
			// TODO: make sure cp.x and cp.y match calculated relative position?
			CompoundPart part;
			static_cast<DullPart&>(part) = sfc_load_entity(cp.entity.get());
			part.relx = cp.relx;
			part.rely = cp.rely;
			parts.push_back(std::move(part));
		}
		for (size_t i = 0; i < hotspots.size(); ++i) {
			hotspots[i] = comp->hotspots[i];
		}
		functions_to_hotspots = comp->functions_to_hotspots;
	}
	Object::serialize(ctx, comp);
}