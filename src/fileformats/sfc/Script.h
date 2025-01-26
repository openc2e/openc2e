#pragma once

#include <stdint.h>
#include <string>

namespace sfc {

struct ScriptV1 {
	// not CArchive serialized, though it likely should have been!
	uint8_t eventno;
	uint8_t species;
	uint8_t genus;
	uint8_t family;
	std::string text;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(eventno);
		ar(species);
		ar(genus);
		ar(family);
		ar.ascii_mfcstring(text);
	}
};

} // namespace sfc