#pragma once

#include "C1Sound.h"
#include "ObjectHandle.h"
#include "RenderableManager.h"
#include "common/Exception.h"
#include "common/FixedPoint.h"

#include <array>

enum MovementStatus {
	MOVEMENT_AUTONOMOUS = 0,
	MOVEMENT_MOUSEDRIVEN = 1,
	MOVEMENT_FLOATING = 2,
	MOVEMENT_INVEHICLE = 3,
	MOVEMENT_CARRIED = 4,
};

enum ActiveFlag : uint8_t {
	ACTV_INACTIVE = 0,
	ACTV_ACTIVE1 = 1,
	ACTV_ACTIVE2 = 2,
};

struct Rect {
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;

	int32_t width() const { return right - left; }
	int32_t height() const { return bottom - top; }
};

class Object {
  public:
	virtual ~Object() = default;
	uint8_t species;
	uint8_t genus;
	uint8_t family;
	MovementStatus movement_status;
	uint8_t attr;
	Rect limit;
	ObjectHandle carrier;
	ActiveFlag actv;
	// creaturesImage sprite;
	int32_t tick_value;
	int32_t ticks_since_last_tick_event;
	ObjectHandle objp;
	C1Sound current_sound;
	uint32_t obv0;
	uint32_t obv1;
	uint32_t obv2;

	ObjectHandle uid;

	virtual RenderableHandle get_part(size_t) { return {}; }
};

class Scenery : public Object {
  public:
	RenderableHandle part;

	RenderableHandle get_part(size_t i) override {
		if (i == 0) {
			return part;
		}
		return {};
	}
};

class SimpleObject : public Object {
  public:
	RenderableHandle part;
	uint32_t z_order;
	std::array<uint8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	RenderableHandle get_part(size_t i) override {
		if (i == 0) {
			return part;
		}
		return {};
	}
};

class PointerTool : public SimpleObject {
  public:
	int32_t relx;
	int32_t rely;
	ObjectHandle bubble;
	std::string text;
};

struct CompoundPart {
	RenderableHandle renderable;
	int32_t x;
	int32_t y;
};

class CompoundObject : public Object {
  public:
	std::vector<CompoundPart> parts;
	std::array<Rect, 6> hotspots;
	std::array<int32_t, 6> functions_to_hotspots;

	RenderableHandle get_part(size_t i) override {
		if (i >= parts.size()) {
			return {};
		}
		return parts[i].renderable;
	}
};

struct Vehicle : public CompoundObject {
	fixed24_8_t xvel;
	fixed24_8_t yvel;
	int32_t cabin_left;
	int32_t cabin_top;
	int32_t cabin_right;
	int32_t cabin_bottom;
	uint32_t bump;
};

inline std::string repr(const Object& o) {
	return fmt::format("<Object cls=({} {} {}) uid={}>", o.family, o.genus, o.species, o.uid);
}

inline std::string repr(const Object* o) {
	if (o == nullptr) {
		return fmt::format("<Object null>");
	}
	return repr(*o);
}