#pragma once

#include "fileformats/MFCObject.h"

#include <stdint.h>
#include <string>
#include <vector>

namespace sfc {

struct CGenomeV1 : MFCObject {
	std::string moniker;
	uint32_t sex;
	uint8_t life_stage;
	std::vector<uint8_t> data;
	// TODO: should we actually parse this? sometimes it has weird extra data
	// that doesn't all get parsed, might be useful to keep it around.

	template <typename Archive>
	void serialize(Archive& ar) {
		ar.size_u32(data);
		ar.ascii_dword(moniker);
		ar(sex);
		ar(life_stage);
		ar(data);
	}
};
} // namespace sfc