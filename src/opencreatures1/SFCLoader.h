#pragma once

#include "ObjectHandle.h"
#include "common/PointerView.h"
#include "fileformats/NewSFCFile.h"

#include <map>

class C1SoundManager;
class ImageManager;
class MapManager;
class ObjectManager;
class RenderableManager;
class Scriptorium;
class ViewportManager;
class MacroManager;

class SFCLoader {
  public:
	SFCLoader(const sfc::SFCFile&);

	void load_everything();

  private:
	void load_viewport();
	void load_map();
	void load_objects();
	void load_scripts();
	void load_macros();

	void object_from_sfc(Object* obj, const sfc::ObjectV1& p);
	void simple_object_from_sfc(Object* obj, const sfc::SimpleObjectV1& p);
	void compound_object_from_sfc(Object* obj, const sfc::CompoundObjectV1& comp);
	void blackboard_from_sfc(Object* obj, const sfc::BlackboardV1& bbd);
	void vehicle_from_sfc(Object* obj, const sfc::VehicleV1& veh);

	std::map<sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;
	const sfc::SFCFile& sfc;
};