#include "ImageManager.h"

#include "EngineContext.h"
#include "PathManager.h"
#include "common/Ascii.h"
#include "common/Exception.h"
#include "common/NumericCast.h"
#include "common/Repr.h"
#include "common/backend/Backend.h"

#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;


ImageManager::ImageManager() {
}

static void build_textures(creaturesImage& image) {
	for (unsigned int frame = 0; frame < image.numframes(); ++frame) {
		if (image.width(frame) > 0 && image.height(frame) > 0 && !image.getTextureForFrame(frame)) {
			image.getTextureForFrame(frame) = get_backend()->createTextureFromImage(image.getImageForFrame(frame));
		}
	}
}

void ImageManager::load_default_palette() {
	auto palette_dta_path = g_engine_context.paths->find_path(PATH_TYPE_IMAGE, "PALETTE.DTA");
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
		path = g_engine_context.paths->find_path(PATH_TYPE_IMAGE, name + ".spr");
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
	build_textures(image);

	m_cache[name] = image;
	return m_cache[name];
}

creaturesImage ImageManager::get_charset_dta(uint32_t bgcolor, uint32_t textcolor, uint32_t aliascolor) {
	// TODO: cache this?

	fs::path path = g_engine_context.paths->find_path(PATH_TYPE_IMAGE, "charset.dta");
	if (path.empty()) {
		throw Exception("Couldn't find charset.dta");
	}

	creaturesImage image("charset.dta");
	image.images = ImageUtils::ReadImage(path);

	// TODO: how do the values in the CHARSET.DTA map to actual color values?
	// just setting them all to the textcolor right now, but the real engines
	// do some shading/aliasing
	for (auto& i : image.images) {
		if (i.format != if_index8) {
			throw Exception("Expected charset.dta to have format if_index8");
		}
		i.palette = m_default_palette;
		for (ptrdiff_t j = 0; j < numeric_cast<ptrdiff_t>(i.data.size()); ++j) {
			if (i.data[j] == 0) {
				i.data[j] = numeric_cast<uint8_t>(bgcolor);
			} else if (i.data[j] == 1) {
				i.data[j] = numeric_cast<uint8_t>(textcolor);
			} else if (i.data[j] > 1) {
				// Creatures 1 has some character pixels > 2 that just read colors
				// randomly from memory. Don't do that, just give them the aliascolor.
				i.data[j] = numeric_cast<uint8_t>(aliascolor);
			}
		}
	}
	build_textures(image);

	return image;
}