#include "SFCLoader.h"

#include "C1SoundManager.h"
#include "EngineContext.h"
#include "ImageManager.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "ObjectManager.h"
#include "PointerManager.h"
#include "Renderable.h"
#include "Scriptorium.h"
#include "ViewportManager.h"

#include <fmt/ranges.h>

static Renderable renderable_from_sfc_entity(sfc::EntityV1& part) {
	if (part.x >= CREATURES1_WORLD_WIDTH) {
		throw Exception(fmt::format("Expected x to be between [0, {}), but got {}", CREATURES1_WORLD_WIDTH, part.x));
	}
	if (part.y >= CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected y to be between [0, {}), but got {}", CREATURES1_WORLD_HEIGHT, part.y));
	}

	Renderable r;
	r.set_position(part.x, part.y);
	r.set_z_order(part.z_order);
	r.set_object_sprite_base(part.sprite->first_sprite);
	r.set_part_sprite_base(part.image_offset);
	r.set_sprite_index(part.current_sprite - part.image_offset);
	r.set_sprite(g_engine_context.images->get_image(part.sprite->filename, ImageManager::IMAGE_SPR));
	if (part.has_animation) {
		r.set_animation(part.animation_frame, part.animation_string);
	}
	return r;
}

void SFCLoader::object_from_sfc(Object* obj, const sfc::ObjectV1& p) {
	obj->species = p.species;
	obj->genus = p.genus;
	obj->family = p.family;
	obj->movement_status = MovementStatus(p.movement_status);
	obj->attr = p.attr;
	obj->limit.x = p.limit_left;
	obj->limit.y = p.limit_top;
	obj->limit.width = p.limit_right - p.limit_left;
	obj->limit.height = p.limit_bottom - p.limit_top;
	obj->carrier = sfc_object_mapping[p.carrier];
	obj->actv = ActiveFlag(p.actv);
	// creaturesImage sprite;
	obj->tick_value = p.tick_value;
	obj->ticks_since_last_tick_event = p.ticks_since_last_tick_event;
	if (!p.current_sound.empty()) {
		// TODO: don't start immediately, wait until window appears
		obj->current_sound = g_engine_context.sounds->play_sound(p.current_sound, true);
	}
	obj->objp = sfc_object_mapping[p.objp];
	obj->obv0 = p.obv0;
	obj->obv1 = p.obv1;
	obj->obv2 = p.obv2;
}

void SFCLoader::simple_object_from_sfc(Object* obj, const sfc::SimpleObjectV1& p) {
	obj->simple_data = std::make_unique<SimpleObjectData>();
	obj->simple_data->part = renderable_from_sfc_entity(*p.part);
	obj->simple_data->z_order = p.z_order;
	obj->simple_data->click_bhvr = p.click_bhvr;
	obj->simple_data->touch_bhvr = p.touch_bhvr;
}

void SFCLoader::compound_object_from_sfc(Object* obj, const sfc::CompoundObjectV1& comp) {
	obj->compound_data = std::make_unique<CompoundObjectData>();

	for (auto& cp : comp.parts) {
		CompoundPart part;
		part.renderable = renderable_from_sfc_entity(*cp.entity);
		part.x = cp.x;
		part.y = cp.y;
		obj->compound_data->parts.emplace_back(std::move(part));
	}
	for (size_t i = 0; i < obj->compound_data->hotspots.size(); ++i) {
		obj->compound_data->hotspots[i].x = comp.hotspots[i].left;
		obj->compound_data->hotspots[i].y = comp.hotspots[i].top;
		obj->compound_data->hotspots[i].width = comp.hotspots[i].right - comp.hotspots[i].left;
		obj->compound_data->hotspots[i].height = comp.hotspots[i].bottom - comp.hotspots[i].top;
	}
	for (size_t i = 0; i < obj->compound_data->functions_to_hotspots.size(); ++i) {
		obj->compound_data->functions_to_hotspots[i] = comp.functions_to_hotspots[i];
	}
}

SFCLoader::SFCLoader(const sfc::SFCFile& sfc_)
	: sfc(sfc_) {
	sfc_object_mapping[nullptr] = {};
}

void SFCLoader::load_everything() {
	fmt::print("INFO [SFCLoader] Loading map...\n");
	load_map();

	fmt::print("INFO [SFCLoader] Loading objects and sceneries...\n");
	load_objects();

	fmt::print("INFO [SFCLoader] Loading scriptorium...\n");
	load_scripts();

	fmt::print("INFO [SFCLoader] Loading viewport...\n");
	load_viewport();

	fmt::print("WARN [SFCLoader] Unsupported: current norn\n");
	fmt::print("WARN [SFCLoader] Unsupported: favorite places\n");
	fmt::print("WARN [SFCLoader] Unsupported: speech history\n");

	fmt::print("INFO [SFCLoader] Loading macros...\n");
	load_macros();

	fmt::print("WARN [SFCLoader] Unsupported: death row objects\n");
	fmt::print("WARN [SFCLoader] Unsupported: event objects\n");
	fmt::print("WARN [SFCLoader] Unsupported: current score\n");
	fmt::print("WARN [SFCLoader] Unsupported: current health\n");
	fmt::print("WARN [SFCLoader] Unsupported: hatchery eggs\n");
	fmt::print("WARN [SFCLoader] Unsupported: natural eggs\n");
	fmt::print("WARN [SFCLoader] Unsupported: dead norns\n");
	fmt::print("WARN [SFCLoader] Unsupported: live norns\n");
	fmt::print("WARN [SFCLoader] Unsupported: breeder score\n");
	fmt::print("WARN [SFCLoader] Unsupported: system tick\n");
	fmt::print("WARN [SFCLoader] Unsupported: stuffed norns\n");
}

void SFCLoader::load_viewport() {
	g_engine_context.viewport->set_scroll_position(sfc.scrollx, sfc.scrolly);
}

void SFCLoader::load_map() {
	if (sfc.map->unused_is_wrappable != 0) {
		fmt::print("WARN [SFCLoader] unused_is_wrappable = {}, expected 0\n", sfc.map->unused_is_wrappable);
	}
	fmt::print("WARN [SFCLoader] Unsupported: time_of_day = {}\n", sfc.map->time_of_day);

	fmt::print("INFO [SFCLoader] Loading background = {}.spr\n", sfc.map->background->filename);
	auto background = g_engine_context.images->get_image(sfc.map->background->filename, ImageManager::IMAGE_SPR);
	// TODO: do any C1 metarooms have non-standard sizes?
	if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
	}
	g_engine_context.map->set_background(background);
	g_engine_context.map->set_groundlevel(sfc.map->groundlevel);

	fmt::print("INFO [SFCLoader] Loading rooms...\n");
	for (auto& r : sfc.map->rooms) {
		Room room;
		room.left = r.left;
		room.top = r.top;
		room.right = r.right;
		room.bottom = r.bottom;
		room.type = r.type;
		g_engine_context.map->add_room(std::move(room));
	}

	fmt::print("WARN [SFCLoader] Unsupported: bacteria\n");
}

void SFCLoader::vehicle_from_sfc(Object* obj, const sfc::VehicleV1& veh) {
	obj->vehicle_data = std::make_unique<VehicleData>();

	obj->vehicle_data->xvel = veh.xvel_times_256 / 256.f;
	obj->vehicle_data->yvel = veh.yvel_times_256 / 256.f;

	auto obj_x = obj->compound_data->parts[0].renderable.get_x();
	auto obj_y = obj->compound_data->parts[0].renderable.get_y();
	auto veh_x = veh.x_times_256 / 256.f;
	auto veh_y = veh.y_times_256 / 256.f;
	if (obj_x != veh_x || obj_y != veh_y) {
		fmt::print("INFO [SFCLoader] Object {} {} {} position {}, {} overridden by VehicleData {}, {}\n", obj->family, obj->genus, obj->species, obj_x, obj_y, veh_x, veh_y);
		obj->compound_data->parts[0].renderable.set_position(veh_x, veh_y);
		fmt::print("TODO: should we add the floating point bit to other parts' positions as well?\n");
	}

	obj->vehicle_data->cabin_left = veh.cabin_left;
	obj->vehicle_data->cabin_top = veh.cabin_top;
	obj->vehicle_data->cabin_right = veh.cabin_right;
	obj->vehicle_data->cabin_bottom = veh.cabin_bottom;
	obj->vehicle_data->bump = veh.bump;
}

void SFCLoader::lift_from_sfc(Object* obj, const sfc::LiftV1& lift) {
	obj->lift_data = std::make_unique<LiftData>();

	obj->lift_data->next_or_current_floor = lift.next_or_current_floor;
	// obj->lift_data->current_call_button = lift.current_call_button;
	// obj->lift_data->delay_counter = lift.delay_counter;

	for (size_t i = 0; i < numeric_cast<size_t>(lift.num_floors); ++i) {
		obj->lift_data->floors.push_back({lift.floors[i].y,
			sfc_object_mapping[lift.floors[i].call_button]});
	}
}

void SFCLoader::blackboard_from_sfc(Object* obj, const sfc::BlackboardV1& bbd) {
	obj->blackboard_data = std::make_unique<BlackboardData>();

	obj->blackboard_data->background_color = bbd.background_color;
	obj->blackboard_data->chalk_color = bbd.chalk_color;
	obj->blackboard_data->alias_color = bbd.alias_color;
	obj->blackboard_data->text_x_position = bbd.text_x_position;
	obj->blackboard_data->text_y_position = bbd.text_y_position;
	for (size_t i = 0; i < bbd.words.size(); ++i) {
		auto& word = bbd.words[i];
		obj->blackboard_data->words[i].value = word.value;
		obj->blackboard_data->words[i].text = word.text;
	}
	obj->blackboard_data->charset_sprite = g_engine_context.images->get_charset_dta(bbd.background_color, bbd.chalk_color, bbd.alias_color);
}

void SFCLoader::load_objects() {
	// first, create empty toplevel objects
	for (auto* p : sfc.objects) {
		auto handle = g_engine_context.objects->add();
		sfc_object_mapping[p] = handle;
	}
	for (auto* p : sfc.sceneries) {
		auto handle = g_engine_context.objects->add();
		sfc_object_mapping[p] = handle;
	}

	// second, load data, including cross-object references
	for (auto* p : sfc.objects) {
		auto handle = sfc_object_mapping[p];
		auto* obj = g_engine_context.objects->try_get(handle);

		object_from_sfc(obj, *p);

		if (auto* simp = dynamic_cast<sfc::SimpleObjectV1*>(p)) {
			simple_object_from_sfc(obj, *simp);
			if (dynamic_cast<sfc::BubbleV1*>(p)) {
				fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type: Bubble\n", obj->family, obj->genus, obj->species);
			}
			if (auto* cb = dynamic_cast<sfc::CallButtonV1*>(p)) {
				obj->call_button_data = std::make_unique<CallButtonData>();
				obj->call_button_data->lift = sfc_object_mapping[cb->lift];
				obj->call_button_data->floor = cb->floor;
			}
			if (auto* pt = dynamic_cast<sfc::PointerToolV1*>(p)) {
				obj->pointer_data = std::make_unique<PointerToolData>();
				obj->pointer_data->relx = pt->relx;
				obj->pointer_data->rely = pt->rely;
				obj->pointer_data->bubble = sfc_object_mapping[pt->bubble];
				obj->pointer_data->text = pt->text;

				g_engine_context.pointer->m_pointer_tool = handle;
			}
		}

		else if (auto* comp = dynamic_cast<sfc::CompoundObjectV1*>(p)) {
			compound_object_from_sfc(obj, *comp);
			if (auto* veh = dynamic_cast<sfc::VehicleV1*>(p)) {
				vehicle_from_sfc(obj, *veh);
				if (auto* lift = dynamic_cast<sfc::LiftV1*>(p)) {
					lift_from_sfc(obj, *lift);
				}
			}
			if (auto* bbd = dynamic_cast<sfc::BlackboardV1*>(p)) {
				blackboard_from_sfc(obj, *bbd);
			}
		}

		else if (dynamic_cast<sfc::CreatureV1*>(p)) {
			fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Creature\n", obj->family, obj->genus, obj->species);
		}
	}
	for (auto* p : sfc.sceneries) {
		auto* obj = g_engine_context.objects->try_get(sfc_object_mapping[p]);

		object_from_sfc(obj, *p);

		obj->scenery_data = std::make_unique<SceneryData>();
		obj->scenery_data->part = renderable_from_sfc_entity(*p->part);
	}

	// Fix up objects
	for (auto& obj : *g_engine_context.objects) {
		// refresh controlled sound positions
		if (obj->current_sound) {
			auto bbox = obj->get_bbox();
			obj->current_sound.set_position(bbox.x, bbox.y, bbox.width, bbox.height);
		}
	}
}

void SFCLoader::load_scripts() {
	for (auto& s : sfc.scripts) {
		g_engine_context.scriptorium->add(s.family, s.genus, s.species, s.eventno, s.text);
	}
}

void SFCLoader::load_macros() {
	for (auto* m : sfc.macros) {
		Macro macro;
		macro.selfdestruct = m->selfdestruct;
		macro.inst = m->inst;
		macro.script = m->script;
		macro.ip = m->ip;
		for (size_t i = 0; i < m->sp; ++i) {
			macro.stack.push_back(m->stack[i]);
		}
		macro.vars = m->vars;
		macro.ownr = sfc_object_mapping[m->ownr];
		macro.from = sfc_object_mapping[m->from];
		macro.exec = sfc_object_mapping[m->exec];
		macro.targ = sfc_object_mapping[m->targ];
		macro._it_ = sfc_object_mapping[m->_it_];
		macro.part = m->part;
		macro.subroutine_label = m->subroutine_label;
		macro.subroutine_address = m->subroutine_address;
		macro.wait = m->wait;

		g_engine_context.macros->add(macro);
	}
}