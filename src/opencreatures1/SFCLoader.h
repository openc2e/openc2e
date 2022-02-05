#pragma once

#include "ObjectHandle.h"
#include "fileformats/NewSFCFile.h"

#include <map>


class ImageManager;
class MapManager;
class ObjectManager;
class RenderableManager;
class Scriptorium;
class ViewportManager;
class MacroManager;

class SFCLoader {
  public:
	SFCLoader(const sfc::SFCFile& sfc);

	void load_viewport(ViewportManager& viewport);
	void load_map(MapManager& map);
	void load_objects(ObjectManager& objects, RenderableManager& renderables, ImageManager& images);
	void load_scripts(Scriptorium& scriptorium);
	void load_macros(MacroManager& macros);

	std::map<sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;

  private:
	const sfc::SFCFile& sfc;
};