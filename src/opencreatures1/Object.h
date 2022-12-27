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
#include "common/math/Rect.h"

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

enum TouchBehaviourFlags : uint8_t {
	TOUCH_ACTIVATE1 = 1,
	TOUCH_ACTIVATE2 = 2,
	TOUCH_DEACTIVATE = 4
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

class BubbleData {
	// TODO: implement me
};

class CallButtonData {
	// TODO: implement me
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

struct LiftData {
	// TODO: implement me
};

struct BlackboardData {
	struct BlackboardWord {
		uint32_t value = 0;
		std::string text;
	};

	uint8_t background_color = 0;
	uint8_t chalk_color = 0;
	uint8_t alias_color = 0;
	int8_t text_x_position = 0;
	int8_t text_y_position = 0;
	std::array<BlackboardWord, 16> words;

	creaturesImage charset_sprite;
	std::array<RenderItemHandle, 11> text_render_items;
};

struct CreatureData {
	// TODO: implement me
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
	std::unique_ptr<BubbleData> bubble_data;
	std::unique_ptr<CallButtonData> call_button_data;

	std::unique_ptr<CompoundObjectData> compound_data;
	std::unique_ptr<VehicleData> vehicle_data;
	std::unique_ptr<LiftData> lift_data;
	std::unique_ptr<BlackboardData> blackboard_data;

	std::unique_ptr<CreatureData> creature_data;

	void handle_left_click(int32_t relx, int32_t rely);

	void handle_mesg_activate1(Message);
	void handle_mesg_activate2(Message);
	void handle_mesg_deactivate(Message);
	void handle_mesg_hit(Message);
	void handle_mesg_pickup(Message);
	void handle_mesg_drop(Message);

	void set_position(fixed24_8_t x, fixed24_8_t y);
	void add_position(fixed24_8_t xdiff, fixed24_8_t ydiff);

	int32_t get_z_order() const;
	Rect get_bbox() const;
	Renderable* get_renderable_for_part(int32_t partnum);
	const Renderable* get_renderable_for_part(int32_t partnum) const;

	void blackboard_show_word(int32_t word_index);
	void blackboard_hide_word();
	void blackboard_enable_edit();
	void blackboard_disable_edit();
	void blackboard_emit_eyesight(int32_t word_index);
	void blackboard_emit_earshot(int32_t word_index);

	void vehicle_grab_passengers();
	void vehicle_drop_passengers();

	void tick();
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