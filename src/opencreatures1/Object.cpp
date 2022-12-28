#include "Object.h"

#include "ObjectManager.h"
#include "common/Ascii.h"
#include "common/NumericCast.h"

static bool world_has_at_least_one_creature() {
	for (auto& obj : *g_engine_context.objects) {
		if (obj->creature_data) {
			return true;
		}
	}
	return false;
}

void Object::handle_left_click(int32_t relx, int32_t rely) {
	// When an object is the subject of a left click event, we queue up a message
	// to ACTIVATE1, ACTIVATE2, or DEACTIVATE. But how do we know which message
	// to send?

	if (pointer_data) {
		throw Exception("handle_click erroneously called on PointerTool");
	}

	// SimpleObjects have their click behavior controlled by BHVR - we index into
	// their click BHVR array using their current ACTV (activation status), which
	// gives us the message to send.
	if (simple_data) {
		// fmt::print("Got a click, click bhvr {} {} {}, actv {}\n", simple_data->click_bhvr[0], simple_data->click_bhvr[1], simple_data->click_bhvr[2], actv);

		int8_t click_message = simple_data->click_bhvr[actv];
		// fmt::print("click message {}\n", click_message);
		if (click_message == -1) {
			// TODO: should we still make the mouse gesture?
			return;
		}
		g_engine_context.events->mesg_writ(g_engine_context.pointer->m_pointer_tool, this->uid, MessageNumber(click_message));

		// TODO: let objects override the pointer script when they get clicked on
		g_engine_context.events->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, SCRIPT_POINTER_ACTIVATE1);
		return;
	}

	// CompoundObjects are completely different. We check the clickable knobs
	// (out of the six total knobs, the first three are for creatures, the
	// second three are for the mouse) and their associated hotspots to see if
	// any contain the click location.
	if (compound_data) {
		// printf("handle_left_click %i %i\n", relx, rely);

		for (size_t i = 3; i < 6; ++i) {
			int32_t hotspot_idx = compound_data->functions_to_hotspots[i];
			if (hotspot_idx == -1 || hotspot_idx < 0) {
				// knob doesn't have hotspot attached
				continue;
			}
			Rect hotspot = compound_data->hotspots[numeric_cast<size_t>(hotspot_idx)];
			// TODO: check for bad hotspots?

			if (hotspot.has_point(relx, rely)) {
				// Found a clickable knob whose hotspot contains this click!
				g_engine_context.events->mesg_writ(g_engine_context.pointer->m_pointer_tool, this->uid, MessageNumber(i - 3));

				// TODO: let objects override the pointer script when they get clicked on
				g_engine_context.events->queue_script(g_engine_context.pointer->m_pointer_tool, g_engine_context.pointer->m_pointer_tool, SCRIPT_POINTER_ACTIVATE1);
				return;
			}
		}
		// TODO: what if there are no knobs/hotspots?
		return;
	}
	throw Exception("handle_click not implemented");
}

void Object::handle_mesg_activate1(Message msg) {
	auto* from = g_engine_context.objects->try_get(msg.from);
	if (pointer_data) {
		throw Exception("handle_mesg_activate1 not implemented on pointer");
	}
	if (simple_data) {
		if (actv == ACTV_ACTIVE1 || (from && from->creature_data && !(simple_data->touch_bhvr & TOUCH_ACTIVATE1))) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (call_button_data) {
			auto* lift = g_engine_context.objects->try_get(call_button_data->lift);
			if (lift->actv == ACTV_INACTIVE && lift->lift_data->next_or_current_floor == call_button_data->floor) {
				// already here, don't stim disappointment though
				return;
			}

			// tell lift to come here eventually
			lift->lift_data->floors[call_button_data->floor].call_button = this->uid;
		}
		actv = ACTV_ACTIVE1;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE1);
		return;
	}
	if (compound_data) {
		if (actv == ACTV_ACTIVE1 || (from && from->creature_data && compound_data->functions_to_hotspots[HOTSPOT_CREATUREACTIVATE1] == -1)) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (lift_data) {
			if (actv != ACTV_INACTIVE || obv0 != 0 || lift_data->next_or_current_floor + 1 >= lift_data->floors.ssize()) {
				// different allowed check, must be ACTV = INACTIVE, OBV0 == 0, and lower floor
				if (from)
					from->creature_stim_disappoint();
				return;
			}
			lift_data->next_or_current_floor++;
		}

		actv = ACTV_ACTIVE1;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE1);
		return;
	}
	throw Exception(fmt::format("handle_mesg_activate1 not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_activate2(Message msg) {
	auto* from = g_engine_context.objects->try_get(msg.from);
	if (pointer_data) {
		throw Exception("handle_mesg_activate2 not implemented on pointer");
	}
	if (simple_data) {
		if (actv == ACTV_ACTIVE2 || (from && from->creature_data && !(simple_data->touch_bhvr & TOUCH_ACTIVATE2))) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (call_button_data) {
			// Call buttons never activate2, always activate1
			return handle_mesg_activate1(msg);
		}
		actv = ACTV_ACTIVE2;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE2);
		return;
	}
	if (compound_data) {
		if (actv == ACTV_ACTIVE2 || (from && from->creature_data && compound_data->functions_to_hotspots[HOTSPOT_CREATUREACTIVATE2] == -1)) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (lift_data) {
			if (actv != ACTV_INACTIVE || obv0 != 0 || lift_data->next_or_current_floor == 0) {
				// different allowed check, must be ACTV = INACTIVE, OBV0 == 0, and higher floor
				if (from)
					from->creature_stim_disappoint();
				return;
			}
			// TODO: maybe set it to ACTV_ACTIVE1 instead?
			lift_data->next_or_current_floor--;
		}

		actv = ACTV_ACTIVE2;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE2);
		return;
	}
	throw Exception(fmt::format("handle_mesg_activate2 not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_deactivate(Message msg) {
	auto* from = g_engine_context.objects->try_get(msg.from);
	if (pointer_data) {
		throw Exception("handle_mesg_deactivate not implemented on pointer");
	}
	if (simple_data) {
		if (actv == ACTV_INACTIVE || (from && from->creature_data && !(simple_data->touch_bhvr & TOUCH_DEACTIVATE))) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}

		actv = ACTV_INACTIVE;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_DEACTIVATE);
		return;
	}
	if (compound_data) {
		if (actv == ACTV_INACTIVE || (from && from->creature_data && compound_data->functions_to_hotspots[HOTSPOT_CREATUREDEACTIVATE] == -1)) {
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (lift_data) {
			// not allowed!!
			if (from)
				from->creature_stim_disappoint();
			return;
		}
		if (vehicle_data) {
			// TODO: truncate fixed point position?
			// stop!
			vehicle_data->xvel = 0;
			vehicle_data->yvel = 0;
		}

		actv = ACTV_INACTIVE;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_DEACTIVATE);
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

void Object::set_position(fixed24_8_t newx, fixed24_8_t newy) {
	// TODO: if the object's current x-position is 100.1 and the new x-position
	// is 100 (low-precision), should we actually move it? do we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	auto* main_part = get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*this));
	}

	// TODO: replace this with get_position and a Vector2?
	if (main_part->get_x() == newx && main_part->get_y() == newy) {
		return;
	}

	// update compound parts
	if (auto* comp = compound_data.get()) {
		for (size_t i = 1; i < comp->parts.size(); ++i) {
			Renderable& p = comp->parts[i].renderable;

			fixed24_8_t relx = p.get_x() - main_part->get_x();
			fixed24_8_t rely = p.get_y() - main_part->get_y();

			p.set_position(newx + relx, newy + rely);
		}
	}

	// now set main part position
	main_part->set_position(newx, newy);

	// update any controlled sound
	if (current_sound) {
		auto bbox = get_bbox();
		current_sound.set_position(bbox.x, bbox.y, bbox.width, bbox.height);
	}
}

void Object::add_position(fixed24_8_t xdiff, fixed24_8_t ydiff) {
	// TODO: if the object's current x-position is 100.1 and the x-diff is 5 (low-
	// precision), do we move it to 105.1 or 105? e.g. should we lose the
	// high-precision part of the current position? This is only relevant to Vehicles.

	auto* main_part = get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Tried to move an object without any parts: {}", repr(*this));
	}
	if (xdiff == 0 && ydiff == 0) {
		return;
	}
	set_position(main_part->get_x() + xdiff, main_part->get_y() + ydiff);
}

int32_t Object::get_z_order() const {
	auto* main_part = get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Can't get main part of object without any parts: {}", repr(*this));
	}

	return main_part->get_z_order();
}

Rect Object::get_bbox() const {
	auto* main_part = get_renderable_for_part(0);
	if (!main_part) {
		throw_exception("Can't get main part of object without any parts: {}", repr(*this));
	}

	return main_part->get_bbox();
}

Renderable* Object::get_renderable_for_part(int32_t partnum) {
	return const_cast<Renderable*>(const_cast<const Object*>(this)->get_renderable_for_part(partnum));
}

const Renderable* Object::get_renderable_for_part(int32_t partnum) const {
	if (scenery_data) {
		if (partnum == 0) {
			return &scenery_data->part;
		}
	} else if (simple_data) {
		if (partnum == 0) {
			return &simple_data->part;
		}
	} else if (compound_data) {
		auto idx = numeric_cast<uint32_t>(partnum);
		if (idx >= compound_data->parts.size()) {
			return {};
		}
		return &compound_data->parts[idx].renderable;
	}
	return nullptr;
}

void Object::blackboard_show_word(int32_t word_index) {
	if (word_index < 0 || numeric_cast<size_t>(word_index) >= blackboard_data->words.size()) {
		throw Exception(fmt::format("Blackboard word index {} out of bounds", word_index));
	}
	const auto& word = blackboard_data->words[numeric_cast<size_t>(word_index)];

	int32_t x = get_bbox().x + blackboard_data->text_x_position;
	const int32_t y = get_bbox().y + blackboard_data->text_y_position;
	// TODO: What should the actual z-order be? Should this be in a SortingGroup with the
	// main blackboard part?
	const int32_t z = get_z_order() + 1;

	blackboard_hide_word();
	for (size_t i = 0; i < word.text.size(); ++i) {
		const unsigned int frame = static_cast<unsigned char>(to_ascii_uppercase(word.text[i]));
		const auto texture = blackboard_data->charset_sprite.getTextureForFrame(frame);

		auto renderitem = g_engine_context.rendersystem->render_item_create(LAYER_OBJECTS);
		g_engine_context.rendersystem->render_item_set_texture(renderitem, texture);
		g_engine_context.rendersystem->render_item_set_position(renderitem, x, y, z);
		blackboard_data->text_render_items[i] = std::move(renderitem);

		x += blackboard_data->charset_sprite.width(frame) + 1;
	}
}

void Object::blackboard_hide_word() {
	blackboard_data->text_render_items = {};
}

void Object::blackboard_enable_edit() {
	printf("WARNING: blackboard_enable_edit not implemented\n");
}

void Object::blackboard_disable_edit() {
	// TODO: no-op until we implement blackboard_enable_edit
}

void Object::blackboard_emit_eyesight(int32_t word_index) {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: blackboard_emit_eyesight %i not implemented\n", word_index);
	}
}

void Object::blackboard_emit_earshot(int32_t word_index) {
	if (world_has_at_least_one_creature()) {
		printf("WARNING: blackboard_emit_earshot %i not implemented\n", word_index);
	}
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
	// TODO: handle this in a separate VehicleSystem or VelocitySystem or PhysicsSystem or something?
	if (vehicle_data) {
		if (lift_data && vehicle_data->yvel != 0) {
			auto current_cabin_bottom = get_bbox().y + vehicle_data->cabin_bottom;
			auto next_cabin_bottom = current_cabin_bottom + vehicle_data->yvel;

			auto next_floor_y = lift_data->floors[lift_data->next_or_current_floor].y;

			if ((vehicle_data->yvel > 0 && next_cabin_bottom >= next_floor_y) || (vehicle_data->yvel < 0 && next_cabin_bottom <= next_floor_y)) {
				set_position(get_bbox().x + vehicle_data->xvel, next_floor_y - vehicle_data->cabin_bottom);
				// stop!
				vehicle_data->xvel = 0;
				vehicle_data->yvel = 0;
				if (actv != ACTV_INACTIVE) {
					actv = ACTV_INACTIVE;
					g_engine_context.events->queue_script(this, this, SCRIPT_DEACTIVATE);
				}

				if (auto call_button = lift_data->floors[lift_data->next_or_current_floor].call_button) {
					lift_data->floors[lift_data->next_or_current_floor].call_button = {};
					g_engine_context.objects->try_get(call_button)->actv = ACTV_INACTIVE;
					g_engine_context.events->queue_script(this->uid, call_button, SCRIPT_DEACTIVATE);
				}
				return;
			}
		}

		add_position(vehicle_data->xvel, vehicle_data->yvel);
	}

	if (lift_data && actv == ACTV_INACTIVE && obv0 == 0) {
		// any callers?

		auto current_cabin_bottom = get_bbox().y + vehicle_data->cabin_bottom;

		int32_t best_y;
		int32_t best_floor;
		ObjectHandle best_call_button;
		for (size_t i = 0; i < lift_data->floors.size(); ++i) {
			auto& floor = lift_data->floors[i];
			if (floor.call_button) {
				if (!g_engine_context.objects->try_get(floor.call_button)) {
					floor.call_button = {};
					continue;
				}
				if (!best_call_button || abs(current_cabin_bottom - floor.y) < abs(current_cabin_bottom - best_y)) {
					best_y = floor.y;
					best_floor = numeric_cast<int32_t>(i);
					best_call_button = floor.call_button;
				}
			}
		}
		if (best_call_button) {
			if (best_y >= current_cabin_bottom) {
				lift_data->next_or_current_floor = best_floor;
				actv = ACTV_ACTIVE1;
				g_engine_context.events->queue_script(this->uid, this->uid, SCRIPT_ACTIVATE1);
			} else {
				lift_data->next_or_current_floor = best_floor;
				actv = ACTV_ACTIVE2;
				g_engine_context.events->queue_script(this->uid, this->uid, SCRIPT_ACTIVATE2);
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
	if (creature_data) {
		printf("WARNING: creature_stim_disappoint not implemented\n");
	}
}

void Object::creature_stim_writ() {
	if (creature_data) {
		printf("WARNING: creature_stim_writ not implemented\n");
	}
}