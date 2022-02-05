#pragma once

#include "ObjectHandle.h"
#include "RenderableManager.h"
#include "common/Exception.h"

#include <array>

enum MovementStatus {
	MOVEMENT_AUTONOMOUS = 0,
	MOVEMENT_MOUSEDRIVEN = 1,
	MOVEMENT_FLOATING = 2,
	MOVEMENT_INVEHICLE = 3,
	MOVEMENT_CARRIED = 4,
};

struct Rect {
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
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
	uint8_t actv;
	// creaturesImage sprite;
	int32_t tick_value;
	int32_t ticks_since_last_tick_event;
	ObjectHandle objp;
	// std::string current_sound;
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
	std::array<uint32_t, 6> functions_to_hotspots;

	RenderableHandle get_part(size_t i) override {
		if (i >= parts.size()) {
			return {};
		}
		return parts[i].renderable;
	}
};