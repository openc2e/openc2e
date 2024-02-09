#pragma once

#include "C1ControlledSound.h"
#include "EngineContext.h"
#include "MessageManager.h"
#include "ObjectHandle.h"
#include "ObjectNames.h"
#include "PointerManager.h"
#include "Renderable.h"
#include "common/Exception.h"
#include "common/StaticSet.h"
#include "common/StaticVector.h"
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

struct Scenery;
struct SimpleObject;
struct Bubble;
struct CallButton;
struct PointerTool;
struct CompoundObject;
struct Vehicle;
struct Lift;
struct Blackboard;
struct Creature;

class Object {
  public:
	virtual ~Object() = default;
	ObjectHandle uid;

	uint8_t species;
	uint8_t genus;
	uint8_t family;
	MovementStatus movement_status;
	uint8_t attr;
	Rect2i limit;
	ObjectHandle carrier;
	ActiveFlag actv;
	// creaturesImage sprite;
	int32_t tick_value;
	int32_t ticks_since_last_tick_event;
	ObjectHandle objp;
	C1ControlledSound current_sound;
	int32_t obv0;
	int32_t obv1;
	int32_t obv2;

	Scenery* as_scenery();
	SimpleObject* as_simple_object();
	Bubble* as_bubble();
	CallButton* as_call_button();
	PointerTool* as_pointer_tool();
	CompoundObject* as_compound_object();
	Vehicle* as_vehicle();
	Lift* as_lift();
	Blackboard* as_blackboard();
	Creature* as_creature();

	const Scenery* as_scenery() const;
	const SimpleObject* as_simple_object() const;
	const Bubble* as_bubble() const;
	const CallButton* as_call_button() const;
	const PointerTool* as_pointer_tool() const;
	const CompoundObject* as_compound_object() const;
	const Vehicle* as_vehicle() const;
	const Lift* as_lift() const;
	const Blackboard* as_blackboard() const;
	const Creature* as_creature() const;

	void handle_left_click(int32_t relx, int32_t rely);

	void handle_mesg_activate1(Message);
	void handle_mesg_activate2(Message);
	void handle_mesg_deactivate(Message);

	void handle_mesg_hit(Message);
	void handle_mesg_pickup(Message);
	void handle_mesg_drop(Message);

	void set_position(float x, float y);
	void add_position(float xdiff, float ydiff);

	int32_t get_z_order() const;
	Rect2i get_bbox() const;
	Renderable* get_renderable_for_part(int32_t partnum);
	const Renderable* get_renderable_for_part(int32_t partnum) const;

	void blackboard_show_word(int32_t word_index);
	void blackboard_hide_word();
	void blackboard_enable_edit();
	void blackboard_disable_edit();
	void blackboard_emit_eyesight(int32_t word_index);
	void blackboard_emit_earshot(int32_t word_index);

	void stim_shou();
	void stim_sign();

	void creature_stim_writ();
	void creature_stim_disappoint();

	void vehicle_grab_passengers();
	void vehicle_drop_passengers();

	void tick();
};

struct Scenery : Object {
	Renderable part;
};

struct SimpleObject : Object {
	Renderable part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;
};

struct Bubble : SimpleObject {
	// TODO: implement me
};

struct CallButton : SimpleObject {
	ObjectHandle lift;
	uint8_t floor;
};

struct PointerTool : SimpleObject {
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

enum HotspotFunction {
	HOTSPOT_CREATUREACTIVATE1,
	HOTSPOT_CREATUREACTIVATE2,
	HOTSPOT_CREATUREDEACTIVATE,
	HOTSPOT_MOUSEACTIVATE1,
	HOTSPOT_MOUSEACTIVATE2,
	HOTSPOT_MOUSEDEACTIVATE,
};

struct CompoundObject : Object {
	std::vector<CompoundPart> parts;
	std::array<Rect2i, 6> hotspots;
	std::array<int32_t, 6> functions_to_hotspots;
};

struct Vehicle : CompoundObject {
	float xvel;
	float yvel;
	Rect2i cabin;
	uint32_t bump;
};

struct Lift : Vehicle {
	int32_t next_or_current_floor = -1;
	StaticVector<int32_t, 8> floors;
	StaticSet<ObjectHandle, 8> activated_call_buttons;
};

struct Blackboard : CompoundObject {
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

	ImageGallery charset_sprite;
	std::array<RenderItemHandle, 11> text_render_items;
};

struct Creature : Object {
	// TODO: implement me
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