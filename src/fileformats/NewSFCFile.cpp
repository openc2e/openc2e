#include "NewSFCFile.h"

#include <fstream>

namespace sfc {

SFCFile read_sfc_v1_file(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	return read_sfc_v1_file(in);
}

SFCFile read_sfc_v1_file(std::istream& in) {
	// set up types
	MFCReader reader(in);
	reader.register_class<MapDataV1>("MapData", 1);
	reader.register_class<CGalleryV1>("CGallery", 1);
	reader.register_class<PointerToolV1>("PointerTool", 1);
	reader.register_class<EntityV1>("Entity", 1);
	reader.register_class<CompoundObjectV1>("CompoundObject", 1);
	reader.register_class<SimpleObjectV1>("SimpleObject", 1);
	reader.register_class<VehicleV1>("Vehicle", 1);
	reader.register_class<LiftV1>("Lift", 1);
	reader.register_class<SceneryV1>("Scenery", 1);
	reader.register_class<MacroV1>("Macro", 1);
	reader.register_class<BlackboardV1>("Blackboard", 1);
	reader.register_class<CallButtonV1>("CallButton", 1);

	// read file
	SFCFile sfc;
	sfc.map = reader.read_type<MapDataV1>();

	uint32_t num_objects = reader.read32le();
	for (size_t i = 0; i < num_objects; ++i) {
		sfc.objects.push_back(reader.read_type<ObjectV1>());
	}

	uint32_t num_sceneries = reader.read32le();
	for (size_t i = 0; i < num_sceneries; ++i) {
		sfc.sceneries.push_back(reader.read_type<SceneryV1>());
	}

	uint32_t num_scripts = reader.read32le();
	for (size_t i = 0; i < num_scripts; ++i) {
		ScriptV1 script;
		script.read_from(reader);
		sfc.scripts.push_back(script);
	}

	sfc.scrollx = reader.reads32le();
	sfc.scrolly = reader.reads32le();

	sfc.current_norn = reader.read_type<CreatureV1>();

	for (size_t i = 0; i < 6; ++i) {
		FavoritePlaceV1 favplace;
		favplace.name = reader.read_ascii_mfcstring();
		favplace.x = reader.reads16le();
		favplace.y = reader.reads16le();
		sfc.favorite_places.push_back(favplace);
	}

	uint16_t size_speech_history = reader.read16le();
	for (size_t i = 0; i < size_speech_history; ++i) {
		sfc.speech_history.push_back(reader.read_ascii_mfcstring());
	}

	uint32_t num_macros = reader.read32le();
	for (size_t i = 0; i < num_macros; ++i) {
		sfc.macros.push_back(reader.read_type<MacroV1>());
	}

	uint32_t num_death_row = reader.read32le();
	for (size_t i = 0; i < num_death_row; ++i) {
		sfc.death_row.push_back(reader.read_type<ObjectV1>());
	}

	uint32_t num_events = reader.read32le();
	for (size_t i = 0; i < num_events; ++i) {
		sfc.events.push_back(reader.read_type<ObjectV1>());
	}

	sfc.current_score = reader.read32le();
	sfc.current_health = reader.read32le();
	sfc.hatchery_eggs = reader.read32le();
	sfc.natural_eggs = reader.read32le();
	sfc.dead_norns = reader.read32le();
	sfc.live_norns = reader.read32le();
	sfc.breeders_score = reader.read32le();
	sfc.tick = reader.read32le();

	uint32_t num_stuffed_norns = reader.read32le();
	for (size_t i = 0; i < num_stuffed_norns; ++i) {
		sfc.stuffed_norns.push_back(reader.read_type<CreatureV1>());
	}

	sfc.mfc_objects = reader.release_objects();
	return sfc;
}

} // namespace sfc