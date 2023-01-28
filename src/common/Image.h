#pragma once

#include "common/Color.h"
#include "common/shared_array.h"

#include <stdint.h>
#include <vector>


enum imageformat {
	if_index8,
	if_rgb555,
	if_rgb565,
	if_bgr24,
	if_rgb24
};

class Image {
  public:
	int32_t width = 0;
	int32_t height = 0;
	imageformat format;
	shared_array<uint8_t> data;
	shared_array<Color> palette;
	Color colorkey;

	explicit operator bool() const {
		return width > 0 && height > 0 && data;
	}
};

using MultiImage = std::vector<Image>;
