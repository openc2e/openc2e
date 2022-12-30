#pragma once

#include "Rect.h"

#include "common/NumericCast.h"
#include <fmt/core.h>
#include <stdint.h>

struct RectF {
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;

	RectF() {}
	RectF(float x_, float y_, float width_, float height_)
		: x(x_), y(y_), width(width_), height(height_) {}
	RectF(Rect other)
		: x(numeric_cast<float>(other.x)),
		  y(numeric_cast<float>(other.y)),
		  width(numeric_cast<float>(other.width)),
		  height(numeric_cast<float>(other.height)) {}

	float right() const { return x + width; }
	float bottom() const { return y + height; }

	bool has_point(float pointx, float pointy) const {
		return pointx >= x && pointx < right() && pointy >= y && pointy < bottom();
	}

	bool intersects(RectF other) const {
		if (x > other.x + other.width) {
			return false;
		}
		if (x + width < other.x) {
			return false;
		}
		if (y > other.y + other.height) {
			return false;
		}
		if (y + height < other.y) {
			return false;
		}
		return true;
	}

	bool operator==(const RectF& other) const {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	bool operator!=(const RectF& other) const {
		return !(*this == other);
	}
};

template <>
struct fmt::formatter<RectF> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(RectF val, FormatContext& ctx) {
		return format_to(ctx.out(), "RectF({},{} + {},{})", val.x, val.y, val.width, val.height);
	}
};