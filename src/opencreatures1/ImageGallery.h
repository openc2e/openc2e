#pragma once

// based off of openc2e's creaturesImage

#include "common/NumericCast.h"
#include "common/backend/BackendTexture.h"
#include "common/math/Rect.h"

#include <string>
#include <vector>

class ImageGallery {
  public:
	std::string name;
	int32_t absolute_base = 0;
	int32_t image_count = 0;

	// TODO:
	// ideally, this Texture and texture locations / atlas would be extracted and made
	// it's own, shared and cached object.  currently we load all sprites in the file
	// but then only share this ImageGallery between objetcts with the same absolute
	// base and image count.
	// alternately, we could act _more_ like Creatures 1 and only load the sprites that
	// are needed for this gallery.
	Texture texture;
	std::vector<Rect> texture_locations;

	explicit operator bool() const {
		return !name.empty() || texture || !texture_locations.empty();
	}

	int32_t size() const {
		return numeric_cast<int32_t>(texture_locations.size());
	}
	int32_t width(int32_t frame) const {
		return numeric_cast<int32_t>(texture_locations[numeric_cast<size_t>(frame)].width);
	}
	int32_t height(int32_t frame) const {
		return numeric_cast<int32_t>(texture_locations[numeric_cast<size_t>(frame)].height);
	}
	// bool transparentAt(unsigned int frame, unsigned int x, unsigned int y) const;
};