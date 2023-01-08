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

static SpriteGallery build_gallery(const std::string& name, MultiImage images) {
	// stupid simple atlas-ing, can make this better

	unsigned int total_width = 0;
	unsigned int max_height = 0;
	for (const auto& img : images) {
		total_width += img.width;
		if (img.height > max_height) {
			max_height = img.height;
		}
	}

	SpriteGallery gallery;
	gallery.name = name;
	gallery.texture = get_backend()->createTexture(total_width, max_height);

	unsigned int current_x = 0;
	for (const auto& img : images) {
		Rect location{
			numeric_cast<int32_t>(current_x),
			0,
			numeric_cast<int32_t>(img.width),
			numeric_cast<int32_t>(img.height)};
		gallery.texture_locations.push_back(location);
		if (img.width > 0 && img.height > 0) {
			// Creatures 1 has a few sprites with 0x0 size at end of files
			get_backend()->updateTexture(gallery.texture, location, img);
		}
		current_x += img.width;
	}
	return gallery;
}

void ImageManager::load_default_palette() {
	auto palette_dta_path = g_engine_context.paths->find_path(PATH_TYPE_IMAGE, "PALETTE.DTA");
	if (palette_dta_path.empty()) {
		throw Exception("Couldn't find PALETTE.DTA");
	}
	m_default_palette = ReadPaletteFile(palette_dta_path);
}

const SpriteGallery& ImageManager::get_image(std::string name, ImageType allowed_types) {
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

	auto images = ImageUtils::ReadImage(path);
	if (ImageUtils::IsBackground(images)) {
		// TODO: I guess do this here? Instead of being explicit?
		images = {ImageUtils::StitchBackground(images)};
	}
	for (auto& i : images) {
		if (i.format == if_index8 && !i.palette) {
			i.palette = m_default_palette;
		}
	}
	m_cache[name] = build_gallery(name, images);
	return m_cache[name];
}

SpriteGallery ImageManager::get_charset_dta(uint32_t bgcolor, uint32_t textcolor, uint32_t aliascolor) {
	// TODO: cache this?

	fs::path path = g_engine_context.paths->find_path(PATH_TYPE_IMAGE, "charset.dta");
	if (path.empty()) {
		throw Exception("Couldn't find charset.dta");
	}

	auto images = ImageUtils::ReadImage(path);
	for (auto& i : images) {
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

	return build_gallery("charset.dta", images);
}