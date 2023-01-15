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
	reader(sfc.map);

	reader.size_u32(sfc.objects);
	for (auto& o : sfc.objects) {
		reader(o);
	}

	reader.size_u32(sfc.sceneries);
	for (auto& s : sfc.sceneries) {
		reader(s);
	}

	reader.size_u32(sfc.scripts);
	for (auto& script : sfc.scripts) {
		script.serialize(reader);
	}

	reader(sfc.scrollx);
	reader(sfc.scrolly);
	reader(sfc.current_norn);

	for (auto& favplace : sfc.favorite_places) {
		reader.ascii_mfcstring(favplace.name);
		reader(favplace.x);
		reader(favplace.y);
	}

	reader.size_u16(sfc.speech_history);
	for (auto& s : sfc.speech_history) {
		reader.ascii_mfcstring(s);
	}

	reader.size_u32(sfc.macros);
	for (auto& m : sfc.macros) {
		reader(m);
	}

	reader.size_u32(sfc.death_row);
	for (auto& d : sfc.death_row) {
		reader(d);
	}

	reader.size_u32(sfc.events);
	for (auto& e : sfc.events) {
		reader(e);
	}

	reader(sfc.current_score);
	reader(sfc.current_health);
	reader(sfc.hatchery_eggs);
	reader(sfc.natural_eggs);
	reader(sfc.dead_norns);
	reader(sfc.live_norns);
	reader(sfc.breeders_score);
	reader(sfc.tick);

	reader.size_u32(sfc.stuffed_norns);
	for (auto& n : sfc.stuffed_norns) {
		reader(n);
	}

	sfc.mfc_objects = reader.release_objects();
	return sfc;
}

EXPFile read_exp_v1_file(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	return read_exp_v1_file(in);
}

EXPFile read_exp_v1_file(std::istream& in) {
	// set up types
	MFCReader reader(in);
	reader.register_class<CGalleryV1>("CGallery", 1);
	reader.register_class<BodyV1>("Body", 1);
	reader.register_class<LimbV1>("Limb", 1);
	reader.register_class<CBrainV1>("CBrain", 1);
	reader.register_class<CBiochemistryV1>("CBiochemistry", 1);
	reader.register_class<CInstinctV1>("CInstinct", 1);
	reader.register_class<CreatureV1>("Creature", 1);
	reader.register_class<CGenomeV1>("CGenome", 1);

	// read file
	EXPFile exp;
	reader(exp.creature);
	reader(exp.genome);

	if (exp.creature->zygote.size()) {
		reader(exp.child_genome);
	} else {
		exp.child_genome = nullptr;
	}

	exp.mfc_objects = reader.release_objects();
	return exp;
}

} // namespace sfc