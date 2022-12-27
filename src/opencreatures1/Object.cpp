#include "Object.h"

#include "ObjectManager.h"
#include "common/Ascii.h"
#include "common/NumericCast.h"

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
	if (simple_data) {
		if (pointer_data) {
			throw Exception("handle_mesg_activate1 not implemented on pointer");
		}

		// if (msg.from.creature_data) {
		//   if (actv == ACTIVATE1 || !(touch_bhvr & ACTIVATE1)) {
		//      msg.from.stim_disappoint();
		//   }
		// }

		if (call_button_data) {
			// if owner lift already at our floor, ignore
			// tell lift to come here
			// then do normal simple object stuff
			throw Exception("handle_mesg_activate1 not implemented for CallButton");
		}

		actv = ACTV_ACTIVE1;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE1);
		return;
	}

	if (compound_data) {
		if (lift_data) {
			// different allowed check, must be ACTV = INACTIVE, OBJ0 == 0, and lower floor
			// then normal compoundobject stuff
			throw Exception("handle_mesg_activate1 not implemented for Lift");
		}

		// if (msg.from.creature_data) {
		//   if (actv == ACTIVATE1 || !has_hotspot(CREATUREACTIVATE1) || !has_script(ACTIVATE1)) {
		//     msg.from.stim_disappoint();
		//   }
		// }

		if (vehicle_data) {
			// TODO: truncate fixed point position?
		}

		actv = ACTV_ACTIVE1;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE1);
		return;
	}

	throw Exception(fmt::format("handle_mesg_activate1 not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_activate2(Message msg) {
	if (simple_data) {
		if (pointer_data) {
			throw Exception("handle_mesg_activate2 not implemented on pointer");
		}
		if (call_button_data) {
			// Call buttons never activate2, always activate1
			return handle_mesg_activate1(msg);
		}

		// if (msg.from.creature_data) {
		//   if (actv == ACTIVATE2 || !(touch_bhvr & ACTIVATE2)) {
		//      msg.from.stim_disappoint();
		//   }
		// }

		actv = ACTV_ACTIVE2;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE2);
		return;
	}

	if (compound_data) {
		if (lift_data) {
			// different allowed check, must be ACTV = INACTIVE, OBJ0 == 0, and _higher_ floor
			// then normal stuff except we set ACTV to ACTIVE1 even though we call the ACTIVATE2 script!
			throw Exception("handle_mesg_activate1 not implemented for Lift");
		}

		// if (msg.from.creature_data) {
		//   if (actv == ACTIVATE2 || !has_hotspot(CREATUREACTIVATE2) || !has_script(ACTIVATE2)) {
		//     msg.from.stim_disappoint();
		//   }
		// }

		if (vehicle_data) {
			// TODO: truncate fixed point position?
		}

		actv = ACTV_ACTIVE2;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_ACTIVATE2);
		return;
	}

	throw Exception(fmt::format("handle_mesg_activate2 not implemented on object {} {} {}", family, genus, species));
}

void Object::handle_mesg_deactivate(Message msg) {
	if (simple_data) {
		if (pointer_data) {
			throw Exception("handle_mesg_deactivate not implemented on pointer");
		}

		// if (msg.from.creature_data) {
		//   if (actv == INACTIVE || !(touch_bhvr & DEACTIVATE)) {
		//      msg.from.stim_disappoint();
		//   }
		// }

		actv = ACTV_INACTIVE;
		g_engine_context.events->queue_script(msg.from, this->uid, SCRIPT_DEACTIVATE);
		return;
	}

	if (compound_data) {
		if (lift_data) {
			// not allowed!!
			// msg.from.stim_disappoint();
			throw Exception("handle_mesg_deactivate not implemented for Lift");
		}

		// if (msg.from.creature_data) {
		//   if (actv == INACTIVE || !has_hotspot(CREATUREDEACTIVATE) || !has_script(DEACTIVATE)) {
		//     msg.from.stim_disappoint();
		//   }
		// }

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
	printf("WARNING: blackboard_emit_eyesight %i not implemented\n", word_index);
}

void Object::blackboard_emit_earshot(int32_t word_index) {
	printf("WARNING: blackboard_emit_earshot %i not implemented\n", word_index);
}

void Object::vehicle_grab_passengers() {
	printf("WARNING: vehicle_grab_passengers not implemented\n");
}

void Object::vehicle_drop_passengers() {
	printf("WARNING: vehicle_drop_passengers not implemented\n");
}

void Object::tick() {
	// TODO: handle this in a separate VehicleSystem or VelocitySystem or PhysicsSystem or something?
	if (vehicle_data) {
		add_position(vehicle_data->xvel, vehicle_data->yvel);
	}
}
