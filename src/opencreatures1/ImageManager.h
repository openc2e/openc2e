#pragma once

#include "common/Color.h"
#include "common/shared_array.h"
#include "opencreatures1/ImageGallery.h"

#include <map>
#include <stdint.h>
#include <string>


class ImageManager {
  public:
	enum ImageType {
		IMAGE_IS_BACKGROUND = 1,
		IMAGE_SPR = 2,
	};

	ImageManager();
	void load_default_palette();
	const ImageGallery& get_image(std::string name, int32_t absolute_base, int32_t image_count, ImageType allowed_types);
	ImageGallery get_charset_dta(uint32_t bgcolor, uint32_t textcolor, uint32_t aliascolor);

  private:
	struct ImageGalleryKey {
		std::string name;
		int32_t absolute_base;
		int32_t image_count;

		bool operator<(const ImageGalleryKey& other) const {
			// stupid std::map and STL
			if (name != other.name)
				return name < other.name;
			if (absolute_base != other.absolute_base)
				return absolute_base < other.absolute_base;
			return image_count < other.image_count;
		}
	};
	std::map<ImageGalleryKey, ImageGallery> m_cache;
	shared_array<Color> m_default_palette;
};