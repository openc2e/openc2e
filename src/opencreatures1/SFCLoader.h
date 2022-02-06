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

	void load_viewport(PointerView<ViewportManager>);
	void load_map(PointerView<MapManager>);
	void load_objects(PointerView<ObjectManager>, PointerView<RenderableManager>,
		PointerView<ImageManager>, PointerView<C1SoundManager>);
	void load_scripts(PointerView<Scriptorium>);
	void load_macros(PointerView<MacroManager>);

	std::map<sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;

  private:
	const sfc::SFCFile& sfc;
};