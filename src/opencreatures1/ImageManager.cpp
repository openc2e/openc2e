#include "ImageManager.h"

#include "PathManager.h"
#include "common/Ascii.h"
#include "common/Exception.h"
#include "common/Repr.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;


ImageManager::ImageManager(std::shared_ptr<PathManager> path_manager)
	: m_path_manager(path_manager) {
}

void ImageManager::load_default_palette() {
	auto palette_dta_path = m_path_manager->find_path(PATH_TYPE_IMAGE, "PALETTE.DTA");
	if (palette_dta_path.empty()) {
		throw Exception("Couldn't find PALETTE.DTA");
	}
	m_default_palette = ReadPaletteFile(palette_dta_path);
}

creaturesImage& ImageManager::get_image(std::string name, ImageType allowed_types) {
	name = to_ascii_lowercase(name);

	// do we have it loaded already?
	auto it = m_cache.find(name);
	if (it != m_cache.end()) {
		return it->second;
	}

	// nope, try to find it
	fs::path path;
	if (allowed_types & IMAGE_SPR) {
		path = m_path_manager->find_path(PATH_TYPE_IMAGE, name + ".spr");
	}
	if (path.empty()) {
		throw Exception(fmt::format("Couldn't find image {}", repr(name)));
	}
	creaturesImage image(name);
	image.images = ImageUtils::ReadImage(path);
	if (ImageUtils::IsBackground(image.images)) {
		// TODO: I guess do this here? Instead of being explicit?
		image.images = {ImageUtils::StitchBackground(image.images)};
	}
	for (auto& i : image.images) {
		if (i.format == if_index8 && !i.palette) {
			i.palette = m_default_palette;
		}
	}

	m_cache[name] = image;
	return m_cache[name];
}