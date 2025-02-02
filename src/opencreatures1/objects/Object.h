#pragma once

#include "C1ControlledSound.h"
#include "DullPart.h"
#include "EngineContext.h"
#include "MessageManager.h"
#include "ObjectHandle.h"
#include "ObjectNames.h"
#include "PointerManager.h"
#include "common/Exception.h"
#include "common/StaticSet.h"
#include "common/StaticVector.h"
#include "common/math/Rect.h"

#include <array>

class SFCContext;
namespace sfc {
struct ObjectV1;
}

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

inline std::string format_as(ActiveFlag a) {
	return std::to_string(a);
}

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

	void handle_left_click(float relx, float rely);

	void handle_mesg_activate1(Message);
	void handle_mesg_activate2(Message);
	void handle_mesg_deactivate(Message);

	void handle_mesg_hit(Message);
	void handle_mesg_pickup(Message);
	void handle_mesg_drop(Message);

	void set_position(float x, float y);
	void add_position(float xdiff, float ydiff);

	int32_t get_z_order() const;
	Rect2f get_bbox() const;
	DullPart* get_part(int32_t partnum);
	virtual const DullPart* get_part(int32_t partnum) const = 0;

	void stim_shou();
	void stim_sign();

	void creature_stim_writ();
	void creature_stim_disappoint();

	void vehicle_grab_passengers();
	void vehicle_drop_passengers();

	void tick();

	void serialize(SFCContext&, sfc::ObjectV1*);
};

inline std::string format_as(const Object& o) {
	return fmt::format("Object {} {} {} uid {} \"{}\"", o.family, o.genus, o.species, o.uid.to_integral(), get_object_name(&o));
}

inline std::string format_as(const Object* o) {
	// formatting pointers is, unfortunately, not allowed by fmt. provide
	// this function for code to use anyways since it matches the naming
	// convention.
	// is there really no better way to do this??
	if (o == nullptr) {
		return "<Object null>";
	}
	return format_as(*o);
}