#pragma once

#include "common/Exception.h"
#include "fileformats/sfc/Script.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

class Reader;
class Writer;

namespace sfc {

struct CreatureV1;
struct MacroV1;
struct MapDataV1;
struct ObjectV1;
struct SceneryV1;

struct FavoritePlaceV1 {
	// not CArchive serialized
	std::string name;
	int16_t x;
	int16_t y;
};

struct SFCFile {
	std::shared_ptr<MapDataV1> map;
	std::vector<std::shared_ptr<ObjectV1>> objects;
	std::vector<std::shared_ptr<SceneryV1>> sceneries;
	std::vector<ScriptV1> scripts;

	int32_t scrollx;
	int32_t scrolly;

	CreatureV1* current_norn = nullptr;
	std::array<FavoritePlaceV1, 6> favorite_places;
	std::vector<std::string> speech_history;
	std::vector<std::shared_ptr<MacroV1>> macros;
	std::vector<ObjectV1*> death_row;
	std::vector<ObjectV1*> events;

	uint32_t current_score;
	uint32_t current_health;
	uint32_t hatchery_eggs;
	uint32_t natural_eggs;
	uint32_t dead_norns;
	uint32_t live_norns;
	uint32_t breeders_score;
	uint32_t tick;

	std::vector<CreatureV1*> stuffed_norns;

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(map);
		ar.size_u32(objects);
		for (auto& o : objects) {
			ar(o);
		}
		ar.size_u32(sceneries);
		for (auto& s : sceneries) {
			ar(s);
		}
		ar.size_u32(scripts);
		for (auto& script : scripts) {
			script.serialize(ar);
		}
		ar(scrollx);
		ar(scrolly);
		ar(current_norn);
		for (auto& favplace : favorite_places) {
			ar.ascii_mfcstring(favplace.name);
			ar(favplace.x);
			ar(favplace.y);
		}
		ar.size_u16(speech_history);
		for (auto& s : speech_history) {
			ar.ascii_mfcstring(s);
		}
		ar.size_u32(macros);
		for (auto& m : macros) {
			ar(m);
		}
		ar.size_u32(death_row);
		for (auto& d : death_row) {
			ar(d);
		}
		ar.size_u32(events);
		for (auto& e : events) {
			ar(e);
		}
		ar(current_score);
		ar(current_health);
		ar(hatchery_eggs);
		ar(natural_eggs);
		ar(dead_norns);
		ar(live_norns);
		ar(breeders_score);
		ar(tick);
		ar.size_u32(stuffed_norns);
		for (auto& n : stuffed_norns) {
			ar(n);
		}
	}
};

SFCFile read_sfc_v1_file(Reader& in);
SFCFile read_sfc_v1_file(const std::string& path);
void write_sfc_v1_file(Writer& out, SFCFile&);
void write_sfc_v1_file(const std::string& path, SFCFile&);

} // namespace sfc
