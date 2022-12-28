#pragma once

#include <fmt/core.h>
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