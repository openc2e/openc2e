#pragma once

#include "CGallery.h"
#include "Script.h"
#include "WinRect.h"
#include "fileformats/MFCObject.h"

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace sfc {

struct ObjectV1 : MFCObject {
	uint8_t species;
	uint8_t genus;
	uint8_t family;
	uint8_t movement_status;
	uint8_t attr;
	WinRect limit;
	ObjectV1* carrier = nullptr;
	uint8_t actv;
	std::shared_ptr<CGalleryV1> gallery;
	int32_t tick_value;
	int32_t ticks_since_last_tick_event;
	ObjectV1* objp = nullptr;
	std::string current_sound;
	int32_t obv0;
	int32_t obv1;
	int32_t obv2;
	std::vector<ScriptV1> scripts;

	template <typename Archive>
	void serialize(Archive& ar) {
		uint8_t unused_eventno = 0;
		ar(unused_eventno);

		ar(species);
		ar(genus);
		ar(family);
		ar(movement_status);
		ar(attr);
		ar(limit.left);
		ar(limit.top);
		ar(limit.right);
		ar(limit.bottom);
		ar(carrier);
		ar(actv);
		ar(gallery);
		ar(tick_value);
		ar(ticks_since_last_tick_event);
		ar(objp);
		ar.ascii_dword(current_sound);
		ar(obv0);
		ar(obv1);
		ar(obv2);
		ar.size_u32(scripts);
		for (auto& s : scripts) {
			s.serialize(ar);
		}
	}
};

} // namespace sfc