#include "CompoundObject.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/CompoundObject.h"
#include "fileformats/sfc/Entity.h"

const DullPart* CompoundObject::get_part(int32_t partnum) const {
	if (partnum >= 0 && numeric_cast<size_t>(partnum) < parts.size()) {
		return &parts[numeric_cast<size_t>(partnum)];
	}
	return nullptr;
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