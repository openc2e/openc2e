#pragma once

#include "fileformats/ImageUtils.h"
#include "fileformats/paletteFile.h"
#include "opencreatures1/SpriteGallery.h"

#include <map>
#include <memory>

class ImageManager {
  public:
	enum ImageType {
		IMAGE_IS_BACKGROUND = 1,
		IMAGE_SPR = 2,
	};

	ImageManager();
	void load_default_palette();
	const SpriteGallery& get_image(std::string name, ImageType allowed_types);
	SpriteGallery get_charset_dta(uint32_t bgcolor, uint32_t textcolor, uint32_t aliascolor);

  private:
	std::map<std::string, SpriteGallery> m_cache;
	shared_array<Color> m_default_palette;
};