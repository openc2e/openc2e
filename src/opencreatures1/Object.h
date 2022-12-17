#pragma once

#include "C1Sound.h"
#include "EngineContext.h"
#include "EventManager.h"
#include "ObjectHandle.h"
#include "ObjectNames.h"
#include "PointerManager.h"
#include "Renderable.h"
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

enum AttributeFlags : uint8_t {
	ATTR_CARRYABLE = 1,
	ATTR_MOUSEABLE = 2,
	ATTR_ACTIVATEABLE = 4,
	ATTR_CONTAINER = 8,
	ATTR_INVISIBLE = 16,
	ATTR_FLOATABLE = 32,
	ATTR_WALLBOUND = 64,
	ATTR_GROUNDBOUND = 128
};

struct Rect {
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;

	int32_t width() const { return right - left; }
	int32_t height() const { return bottom - top; }
};

struct SceneryData {
	Renderable part;
};

class SimpleObjectData {
  public:
	Renderable part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;
};

class PointerToolData {
  public:
	int32_t relx;
	int32_t rely;
	ObjectHandle bubble;
	std::string text;
};

struct CompoundPart {
	Renderable renderable;
	int32_t x;
	int32_t y;
};

class CompoundObjectData {
  public:
	std::vector<CompoundPart> parts;
	std::array<Rect, 6> hotspots;
	std::array<int32_t, 6> functions_to_hotspots;
};

struct VehicleData {
	fixed24_8_t xvel;
	fixed24_8_t yvel;
	int32_t cabin_left;
	int32_t cabin_top;
	int32_t cabin_right;
	int32_t cabin_bottom;
	uint32_t bump;
};

class Object {
  public:
	virtual ~Object() = default;
	ObjectHandle uid;

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
	int32_t obv0;
	int32_t obv1;
	int32_t obv2;

	std::unique_ptr<SceneryData> scenery_data;

	std::unique_ptr<SimpleObjectData> simple_data;
	std::unique_ptr<PointerToolData> pointer_data;

	std::unique_ptr<CompoundObjectData> compound_data;
	std::unique_ptr<VehicleData> vehicle_data;

	void handle_click();

	void handle_mesg_activate1(Message);
	void handle_mesg_activate2(Message);
	void handle_mesg_deactivate(Message);
	void handle_mesg_hit(Message);
	void handle_mesg_pickup(Message);
	void handle_mesg_drop(Message);

	Renderable* get_renderable_for_part(int32_t partnum);
};

inline std::string repr(const Object& o) {
	return fmt::format("Object {} {} {} uid {} \"{}\"", o.family, o.genus, o.species, o.uid.to_integral(), get_object_name(&o));
}

inline std::string repr(const Object* o) {
	if (o == nullptr) {
		return fmt::format("<Object null>");
	}
	return repr(*o);
}