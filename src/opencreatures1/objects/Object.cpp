#include "Object.h"

#include "Blackboard.h"
#include "Bubble.h"
#include "C1SoundManager.h"
#include "CallButton.h"
#include "CompoundObject.h"
#include "Creature.h"
#include "Lift.h"
#include "MacroManager.h"
#include "MessageManager.h"
#include "ObjectManager.h"
#include "PointerTool.h"
#include "SFCSerialization.h"
#include "Scenery.h"
#include "SimpleObject.h"
#include "Vehicle.h"
#include "common/Ascii.h"
#include "common/NumericCast.h"
#include "common/Ranges.h"
#include "fileformats/sfc/Object.h"

#include <cmath>

static bool world_has_at_least_one_creature() {
	for (auto* obj : *g_engine_context.objects) {
		if (obj->as_creature()) {
			return true;
		}
	}
	return false;
}

Scenery* Object::as_scenery() {
	return dynamic_cast<Scenery*>(this);
}

SimpleObject* Object::as_simple_object() {
	return dynamic_cast<SimpleObject*>(this);
}

Bubble* Object::as_bubble() {
	return dynamic_cast<Bubble*>(this);
}

CallButton* Object::as_call_button() {
	return dynamic_cast<CallButton*>(this);
}

PointerTool* Object::as_pointer_tool() {
	return dynamic_cast<PointerTool*>(this);
}

CompoundObject* Object::as_compound_object() {
	return dynamic_cast<CompoundObject*>(this);
}

Vehicle* Object::as_vehicle() {
	return dynamic_cast<Vehicle*>(this);
}

Lift* Object::as_lift() {
	return dynamic_cast<Lift*>(this);
}

Blackboard* Object::as_blackboard() {
	return dynamic_cast<Blackboard*>(this);
}

Creature* Object::as_creature() {
	return dynamic_cast<Creature*>(this);
}

const Scenery* Object::as_scenery() const {
	return dynamic_cast<const Scenery*>(this);
}

const SimpleObject* Object::as_simple_object() const {
	return dynamic_cast<const SimpleObject*>(this);
}

const Bubble* Object::as_bubble() const {
	return dynamic_cast<const Bubble*>(this);
}

const CallButton* Object::as_call_button() const {
	return dynamic_cast<const CallButton*>(this);
}

const PointerTool* Object::as_pointer_tool() const {
	return dynamic_cast<const PointerTool*>(this);
}

const CompoundObject* Object::as_compound_object() const {
	return dynamic_cast<const CompoundObject*>(this);
}

const Vehicle* Object::as_vehicle() const {
	return dynamic_cast<const Vehicle*>(this);
}

const Lift* Object::as_lift() const {
	return dynamic_cast<const Lift*>(this);
}

const Blackboard* Object::as_blackboard() const {
	return dynamic_cast<const Blackboard*>(this);
}

const Creature* Object::as_creature() const {
	return dynamic_cast<const Creature*>(this);
}

void Object::handle_mesg_activate1(Message msg) {
	auto* from = g_engine_context.objects->try_get(msg.from);
	if (as_pointer_tool()) {
		throw Exception("handle_mesg_activate1 not implemented on pointer");
	}
	if (as_simple_object()) {
		if (from && from->as_creature()) {
			if (as_call_button() && (actv == ACTV_ACTIVE1 || actv == ACTV_ACTIVE2)) {
				return from->creature_stim_disappoint();
			}
			if (actv == ACTV_ACTIVE1 || !(as_simple_object()->touch_bhvr & TOUCH_ACTIVATE1)) {
				return from->creature_stim_disappoint();
			}
		}
		if (as_call_button()) {
			auto* lift = g_engine_context.objects->try_get(as_call_button()->lift);
			if (!lift) {
				// whoops, lift doesn't exist
				printf("WARNING: Tried to activate1 a callbutton with non-existent lift!\n");
				return;
			}
			if (lift->actv == ACTV_INACTIVE && lift->as_lift()->next_or_current_floor == as_call_button()->floor) {
				// already here, don't stim disappointment though
				return;
			}

			// tell lift to come here eventually
			// told the lift to do the thing
			lift->as_lift()->activated_call_buttons.insert(this->uid);
		}
		actv = ACTV_ACTIVE1;
		g_engine_context.macros->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE1);
		return;
	}
	if (as_compound_object()) {
		// TODO: also disappoint and exit early if script doesn't exist
		if (actv == ACTV_ACTIVE1 || (from && from->as_creature() && as_compound_object()->functions_to_hotspots[HOTSPOT_CREATUREACTIVATE1] == -1)) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (as_lift()) {
			if (actv != ACTV_INACTIVE || obv0 != 0 || as_lift()->next_or_current_floor + 1 >= as_lift()->floors.ssize()) {
				// different allowed check, must be ACTV = INACTIVE, OBV0 == 0, and lower floor
				if (from)
					from->creature_stim_disappoint();
				return;
			}
			as_lift()->next_or_current_floor++;
		}

		if (as_vehicle()) {
			// truncate fixed point position
			set_position(std::trunc(get_bbox().x), std::trunc(get_bbox().y));
		}

		actv = ACTV_ACTIVE1;
		g_engine_context.macros->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE1);
		return;
	}
	throw Exception(fmt::format("handle_mesg_activate1 not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_activate2(Message msg) {
	auto* from = g_engine_context.objects->try_get(msg.from);
	if (as_pointer_tool()) {
		throw Exception("handle_mesg_activate2 not implemented on pointer");
	}
	if (as_call_button()) {
		// Call buttons never activate2, always activate1
		return handle_mesg_activate1(msg);
	}
	if (as_simple_object()) {
		if (from && from->as_creature()) {
			if (actv == ACTV_ACTIVE2 || !(as_simple_object()->touch_bhvr & TOUCH_ACTIVATE2)) {
				return from->creature_stim_disappoint();
			}
		}
		actv = ACTV_ACTIVE2;
		g_engine_context.macros->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE2);
		return;
	}
	if (as_compound_object()) {
		// TODO: also disappoint and exit early if script doesn't exist
		if (actv == ACTV_ACTIVE2 || (from && from->as_creature() && as_compound_object()->functions_to_hotspots[HOTSPOT_CREATUREACTIVATE2] == -1)) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (as_lift()) {
			if (actv != ACTV_INACTIVE || obv0 != 0 || as_lift()->next_or_current_floor == 0) {
				// different allowed check, must be ACTV = INACTIVE, OBV0 == 0, and higher floor
				if (from)
					from->creature_stim_disappoint();
				return;
			}
			// TODO: maybe set it to ACTV_ACTIVE1 instead?
			as_lift()->next_or_current_floor--;
		}

		if (as_vehicle()) {
			// truncate fixed point position
			set_position(std::trunc(get_bbox().x), std::trunc(get_bbox().y));
		}

		actv = ACTV_ACTIVE2;
		g_engine_context.macros->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE2);
		return;
	}
	throw Exception(fmt::format("handle_mesg_activate2 not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_deactivate(Message msg) {
	auto* from = g_engine_context.objects->try_get(msg.from);
	if (as_pointer_tool()) {
		throw Exception("handle_mesg_deactivate not implemented on pointer");
	}
	if (as_simple_object()) {
		if (from && from->as_creature()) {
			if (actv == ACTV_INACTIVE || !(as_simple_object()->touch_bhvr & TOUCH_DEACTIVATE)) {
				return from->creature_stim_disappoint();
			}
		}

		actv = ACTV_INACTIVE;
		g_engine_context.macros->queue_script(msg.from, this->uid, SCRIPT_DEACTIVATE);
		return;
	}
	if (as_compound_object()) {
		// TODO: also disappoint and exit early if script doesn't exist
		if (actv == ACTV_INACTIVE || (from && from->as_creature() && as_compound_object()->functions_to_hotspots[HOTSPOT_CREATUREDEACTIVATE] == -1)) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (as_lift()) {
			// not allowed!!
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (as_vehicle()) {
			// stop!
			as_vehicle()->xvel = 0;
			as_vehicle()->yvel = 0;
			// truncate fixed point position
			set_position(std::trunc(get_bbox().x), std::trunc(get_bbox().y));
		}

		actv = ACTV_INACTIVE;
		g_engine_context.macros->queue_script(msg.from, this->uid, SCRIPT_DEACTIVATE);
		return;
	}
	throw Exception(fmt::format("handle_mesg_deactivate not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_hit(Message) {
	throw Exception("handle_mesg_hit not implemented");
}

void Object::handle_mesg_pickup(Message) {
	throw Exception("handle_mesg_pickup not implemented");
}

void Object::handle_mesg_drop(Message) {
	throw Exception("handle_mesg_drop not implemented");
}

void Object::set_position(float newx, float newy) {
	// TODO: if the object's current x-position is 100.1 and the new x-position
	// is 100 (low-precision), should we actually move it? do we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	// TODO: is this correct?
	if (get_bbox().x == newx && get_bbox().y == newy) {
		return;
	}

	// update object position
	if (as_scenery()) {
		as_scenery()->part.set_position(newx, newy);
	} else if (as_simple_object()) {
		as_simple_object()->part.set_position(newx, newy);
	} else if (as_compound_object()) {
		for (auto& p : as_compound_object()->parts) {
			p.set_position(newx + p.relx, newy + p.rely);
		}
	} else {
		throw Exception("set_position not implemented for non-scenery, non-simpleobject, non-compoundobject");
	}

	// update any controlled sound
	if (current_sound) {
		auto bbox = get_bbox();
		current_sound.set_position(bbox.x, bbox.y, bbox.width, bbox.height);
	}
}

void Object::add_position(float xdiff, float ydiff) {
	// TODO: if the object's current x-position is 100.1 and the x-diff is 5, do we move
	// it to 105.1 or 105? e.g. should we lose the high-precision part of the current position?
	// This is only relevant to Vehicles.

	// TODO: is using get_bbox() correct?
	set_position(get_bbox().x + xdiff, get_bbox().y + ydiff);
}

int32_t Object::get_z_order() const {
	auto* main_part = get_part(0);
	if (!main_part) {
		throw_exception("Can't get main part of object without any parts: {}", *this);
	}

	return main_part->z_order();
}

Rect2f Object::get_bbox() const {
	auto* main_part = get_part(0);
	if (!main_part) {
		throw_exception("Can't get main part of object without any parts: {}", *this);
	}

	// TODO: is this correct for CompoundObjects? should we find the max of widths and heights?
	// and find any parts with negative relx/rely?
	return Rect2f{main_part->x(), main_part->y(), numeric_cast<float>(main_part->width()), numeric_cast<float>(main_part->height())};
}

DullPart* Object::get_part(int32_t partnum) {
	return const_cast<DullPart*>(const_cast<const Object*>(this)->get_part(partnum));
}

void Object::vehicle_grab_passengers() {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: vehicle_grab_passengers not implemented\n");
	}
}

void Object::vehicle_drop_passengers() {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: vehicle_drop_passengers not implemented\n");
	}
}

void Object::tick() {
	// TODO: if this function gets too slow, break out various bits into separate Systems that only
	// know about Objects that need to be updated (e.g. an Object would have a TimerSystemHandle into
	// the TimerSystem, and the TimerSystem only updates timer scripts for its managed objects).

	// timer updates
	if (tick_value > 0) {
		ticks_since_last_tick_event += 1;
		if (ticks_since_last_tick_event >= tick_value) {
			ticks_since_last_tick_event = 0;
			if (!g_engine_context.macros->queue_script(this, this, SCRIPT_TIMER)) {
				fmt::print("ERRO [Object::tick] Disabling timer for {}\n", *this);
				tick_value = 0;
			}
			// fmt::print("Fired timer script for {}, {}, {}\n", family, genus, species);
		}
	}

	// animation updates
	for (int32_t partno = 0; true; ++partno) {
		auto* r = get_part(partno);
		if (!r) {
			break;
		}
		r->update_animation();
	}

	// vehicle position updates
	if (as_vehicle()) {
		if (as_lift() && as_vehicle()->yvel != 0) {
			auto current_cabin_bottom = get_bbox().y + as_vehicle()->cabin.bottom();
			auto next_cabin_bottom = current_cabin_bottom + as_vehicle()->yvel;

			auto next_floor_y = as_lift()->floors[as_lift()->next_or_current_floor];

			if ((as_vehicle()->yvel > 0 && next_cabin_bottom >= next_floor_y) || (as_vehicle()->yvel < 0 && next_cabin_bottom <= next_floor_y)) {
				set_position(get_bbox().x + as_vehicle()->xvel, next_floor_y - as_vehicle()->cabin.bottom());
				// stop!
				as_vehicle()->xvel = 0;
				as_vehicle()->yvel = 0;
				// truncate any fractional position
				// TODO: does this result in the correct y position? should we just set it to the next_floor_y + height?
				set_position(std::trunc(get_bbox().x), std::trunc(get_bbox().y));
				if (actv != ACTV_INACTIVE) {
					actv = ACTV_INACTIVE;
					g_engine_context.macros->queue_script(this, this, SCRIPT_DEACTIVATE);
				}
				erase_if(as_lift()->activated_call_buttons, [&](auto& cb) {
					auto call_button = g_engine_context.objects->try_get(cb);
					if (call_button == nullptr) {
						return true;
					}
					if (call_button->as_call_button()->floor == as_lift()->next_or_current_floor) {
						call_button->actv = ACTV_INACTIVE;
						g_engine_context.macros->queue_script(this, call_button, SCRIPT_DEACTIVATE);
						return true;
					}
					return false;
				});
				return;
			}
		}

		add_position(as_vehicle()->xvel, as_vehicle()->yvel);
	}

	// lift caller updates
	if (as_lift() && actv == ACTV_INACTIVE && obv0 == 0) {
		// any callers?

		auto current_cabin_bottom = get_bbox().y + as_vehicle()->cabin.bottom();

		int32_t best_y;
		int32_t best_floor_id;
		ObjectHandle best_call_button;
		erase_if(as_lift()->activated_call_buttons, [&](auto& handle) {
			auto* cb = g_engine_context.objects->try_get(handle);
			if (cb == nullptr) {
				return true;
			}
			auto& floor = as_lift()->floors[cb->as_call_button()->floor];
			if (!best_call_button || abs(current_cabin_bottom - floor) < abs(current_cabin_bottom - best_y)) {
				best_y = floor;
				best_floor_id = numeric_cast<int32_t>(cb->as_call_button()->floor);
				best_call_button = handle;
			}
			return false;
		});

		if (best_call_button) {
			if (best_y >= current_cabin_bottom) {
				as_lift()->next_or_current_floor = best_floor_id;
				actv = ACTV_ACTIVE1;
				g_engine_context.macros->queue_script(this->uid, this->uid, SCRIPT_ACTIVATE1);
			} else {
				as_lift()->next_or_current_floor = best_floor_id;
				actv = ACTV_ACTIVE2;
				g_engine_context.macros->queue_script(this->uid, this->uid, SCRIPT_ACTIVATE2);
			}
		}
	}
}

void Object::stim_shou() {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: stim_shou not implemented\n");
	}
}

void Object::stim_sign() {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: stim_shou not implemented\n");
	}
}


void Object::creature_stim_disappoint() {
	if (as_creature()) {
		printf("WARNING: creature_stim_disappoint not implemented\n");
	}
}

void Object::creature_stim_writ() {
	if (as_creature()) {
		printf("WARNING: creature_stim_writ not implemented\n");
	}
}

void Object::serialize(SFCContext& ctx, sfc::ObjectV1* p) {
	if (ctx.is_storing()) {
		p->species = species;
		p->genus = genus;
		p->family = family;
		p->movement_status = movement_status;
		p->attr = attr;
		p->limit = limit;
		p->carrier = ctx.dump_object(g_engine_context.objects->try_get(carrier)).get();
		p->actv = actv;
		if (p->gallery == nullptr) {
			throw Exception("Expected gallery to be non-null by this point, it should have been"
							" set by a serialization function for a more specialized object type");
		}
		p->tick_value = tick_value;
		p->ticks_since_last_tick_event = ticks_since_last_tick_event;
		p->objp = ctx.dump_object(g_engine_context.objects->try_get(objp)).get();
		if (current_sound.get_looping()) {
			p->current_sound = current_sound.get_name();
		}
		p->obv0 = obv0;
		p->obv1 = obv1;
		p->obv2 = obv2;
		// TODO: I think we can skip scripts here, since they'll be loaded from the global
		// scriptorium anyways?
		// p->scripts = scripts;
	} else {
		species = p->species;
		genus = p->genus;
		family = p->family;
		movement_status = MovementStatus(p->movement_status);
		attr = p->attr;
		limit = p->limit;
		carrier = ctx.load_object(p->carrier);
		actv = ActiveFlag(p->actv);
		// creaturesImage sprite;
		tick_value = p->tick_value;
		ticks_since_last_tick_event = p->ticks_since_last_tick_event;
		// Set the sound later, once we've loaded the object's position from its Entities
		// current_sound = p->current_sound;
		objp = ctx.load_object(p->objp);
		obv0 = p->obv0;
		obv1 = p->obv1;
		obv2 = p->obv2;

		// do this _after_ loading the SimpleObject or CompoundObject data,
		// since only then do we know the object's position / bounding box
		// TODO: should probably somewhere else? like global initialization after everything's been loaded?
		if (!p->current_sound.empty()) {
			// these won't be audible immediately, since the SoundManager thinks
			// they're out of hearing range. once the game starts and the
			// listener viewport gets set these will start being audible.
			current_sound = g_engine_context.sounds->play_controlled_sound(p->current_sound, get_bbox(), true);
		}
	}
}