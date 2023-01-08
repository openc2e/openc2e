#pragma once

#include "common/shared_array.h"

#include <memory>
#include <vector>


enum imageformat {
	if_index8,
	if_rgb555,
	if_rgb565,
	if_bgr24,
	if_rgb24
};

class Color {
  public:
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};

class Image {
  public:
	unsigned int width = 0;
	unsigned int height = 0;
	imageformat format;
	shared_array<uint8_t> data;
	shared_array<Color> palette;
	Color colorkey;

	explicit operator bool() const {
		return width > 0 && height > 0 && data;
	}
};

using MultiImage = std::vector<Image>;
