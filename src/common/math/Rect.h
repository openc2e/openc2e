#pragma once

#include <stdint.h>

struct Rect {
	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;

	int32_t right() const { return x + width; }
	int32_t bottom() const { return y + height; }

	bool operator==(const Rect& other) const {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	bool operator!=(const Rect& other) const {
		return !(*this == other);
	}
};