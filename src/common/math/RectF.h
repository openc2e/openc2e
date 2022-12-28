#pragma once

#include <stdint.h>

struct RectF {
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;

	float right() const { return x + width; }
	float bottom() const { return y + height; }

	bool has_point(float pointx, float pointy) const {
		return pointx >= x && pointx < right() && pointy >= y && pointy < bottom();
	}

	bool operator==(const Rect& other) const {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	bool operator!=(const Rect& other) const {
		return !(*this == other);
	}
};