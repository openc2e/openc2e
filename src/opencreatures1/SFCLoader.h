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

	void load_viewport();
	void load_map();
	void load_objects();
	void load_scripts();
	void load_macros();

	std::map<sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;

  private:
	const sfc::SFCFile& sfc;
};