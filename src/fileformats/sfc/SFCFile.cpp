#include "SFCFile.h"

#include "Blackboard.h"
#include "Body.h"
#include "CBiochemistry.h"
#include "CBrain.h"
#include "CGallery.h"
#include "CGenome.h"
#include "CInstinct.h"
#include "CallButton.h"
#include "CompoundObject.h"
#include "Creature.h"
#include "Entity.h"
#include "Lift.h"
#include "Limb.h"
#include "MFCReader.h"
#include "MFCWriter.h"
#include "Macro.h"
#include "MapData.h"
#include "PointerTool.h"
#include "Scenery.h"
#include "SimpleObject.h"
#include "Vehicle.h"
#include "common/io/FileReader.h"
#include "common/io/FileWriter.h"

namespace sfc {

SFCFile read_sfc_v1_file(const std::string& path) {
	FileReader in(path);
	return read_sfc_v1_file(in);
}

SFCFile read_sfc_v1_file(Reader& in) {
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

	reader.register_class<CreatureV1>("Creature", 1);
	reader.register_class<BodyV1>("Body", 1);
	reader.register_class<LimbV1>("Limb", 1);
	reader.register_class<CBrainV1>("CBrain", 1);
	reader.register_class<CBiochemistryV1>("CBiochemistry", 1);
	reader.register_class<CInstinctV1>("CInstinct", 1);
	reader.register_class<CGenomeV1>("CGenome", 1);

	// read file
	SFCFile sfc;
	sfc.serialize(reader);
	return sfc;
}

void write_sfc_v1_file(const std::string& path, SFCFile& sfc) {
	FileWriter out(path);
	return write_sfc_v1_file(out, sfc);
}

void write_sfc_v1_file(Writer& out, SFCFile& sfc) {
	MFCWriter writer(out);
	writer.register_class<sfc::MapDataV1>("MapData", 1);
	writer.register_class<sfc::CGalleryV1>("CGallery", 1);
	writer.register_class<sfc::PointerToolV1>("PointerTool", 1);
	writer.register_class<sfc::EntityV1>("Entity", 1);
	writer.register_class<sfc::CompoundObjectV1>("CompoundObject", 1);
	writer.register_class<sfc::SimpleObjectV1>("SimpleObject", 1);
	writer.register_class<sfc::VehicleV1>("Vehicle", 1);
	writer.register_class<sfc::LiftV1>("Lift", 1);
	writer.register_class<sfc::SceneryV1>("Scenery", 1);
	writer.register_class<sfc::MacroV1>("Macro", 1);
	writer.register_class<sfc::BlackboardV1>("Blackboard", 1);
	writer.register_class<sfc::CallButtonV1>("CallButton", 1);

	writer.register_class<CreatureV1>("Creature", 1);
	writer.register_class<BodyV1>("Body", 1);
	writer.register_class<LimbV1>("Limb", 1);
	writer.register_class<CBrainV1>("CBrain", 1);
	writer.register_class<CBiochemistryV1>("CBiochemistry", 1);
	writer.register_class<CInstinctV1>("CInstinct", 1);
	writer.register_class<CGenomeV1>("CGenome", 1);

	// write file
	sfc.serialize(writer);
}

} // namespace sfc