#pragma once

#include "common/NumericCast.h"
#include "common/backend/BackendTexture.h"
#include "common/math/Rect.h"

#include <string>
#include <vector>

class ImageGallery {
  public:
	std::string name;
	int32_t absolute_base = 0;
	std::vector<int32_t> offsets;

	// TODO:
	// should we pull out Texture and texture locations / atlas into their own, shared
	// and cached object? then we could load all sprites in the file once and share
	// it between all objects with similar galleries
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