#pragma once

#include <stdint.h>

struct Rect {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;

	int32_t right() const { return x + width; }
	int32_t bottom() const { return y + height; }
};