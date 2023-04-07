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
	// std::vector<Image> images;
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