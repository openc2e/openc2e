#pragma once

#include "WinRect.h"
#include "fileformats/MFCObject.h"

#include <array>
#include <memory>
#include <stdint.h>
#include <vector>

namespace sfc {

struct RoomV1 {
	// not CArchive serialized
	WinRect rect;
	int32_t type;
};

struct BacteriumV1 {
	// not CArchive serialized
	uint8_t state;
	uint8_t antigen;
	uint8_t fatal_level;
	uint8_t infect_level;
	uint8_t toxin1;
	uint8_t toxin2;
	uint8_t toxin3;
	uint8_t toxin4;
};

struct MapDataV1 : MFCObject {
	uint32_t unused_is_wrappable;
	uint32_t time_of_day;
	std::shared_ptr<CGalleryV1> background;
	std::vector<RoomV1> rooms;
	std::array<uint32_t, 261> groundlevel;
	std::array<BacteriumV1, 100> bacteria;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(unused_is_wrappable);
		ar(time_of_day);
		ar(background);

		ar.size_u32(rooms);
		for (auto& room : rooms) {
			ar(room.rect.left);
			ar(room.rect.top);
			ar(room.rect.right);
			ar(room.rect.bottom);
			ar(room.type);
		}
		for (auto& g : groundlevel) {
			ar(g);
		}
		for (auto& b : bacteria) {
			ar(b.state);
			ar(b.antigen);
			ar(b.fatal_level);
			ar(b.infect_level);
			ar(b.toxin1);
			ar(b.toxin2);
			ar(b.toxin3);
			ar(b.toxin4);
		}
	}
};

} // namespace sfc