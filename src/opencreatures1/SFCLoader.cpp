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
	r.x = part.x;
	r.y = part.y;
	r.z = part.z_order;
	r.object_sprite_base = part.sprite->first_sprite;
	r.part_sprite_base = part.image_offset;
	r.sprite_index = part.current_sprite - part.image_offset;
	r.sprite = g_engine_context.images->get_image(part.sprite->filename, ImageManager::IMAGE_SPR);
	r.has_animation = part.has_animation;
	if (part.has_animation) {
		r.animation_frame = part.animation_frame;
		r.animation_string = part.animation_string;
	}
	return r;
}

void SFCLoader::object_from_sfc(Object* obj, const sfc::ObjectV1& p) {
	obj->species = p.species;
	obj->genus = p.genus;
	obj->family = p.family;
	obj->movement_status = MovementStatus(p.movement_status);
	obj->attr = p.attr;
	obj->limit.left = p.limit_left;
	obj->limit.top = p.limit_top;
	obj->limit.right = p.limit_right;
	obj->limit.bottom = p.limit_bottom;
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
		obj->compound_data->parts.push_back(part);
	}
	for (size_t i = 0; i < obj->compound_data->hotspots.size(); ++i) {
		obj->compound_data->hotspots[i].left = comp.hotspots[i].left;
		obj->compound_data->hotspots[i].top = comp.hotspots[i].top;
		obj->compound_data->hotspots[i].right = comp.hotspots[i].right;
		obj->compound_data->hotspots[i].bottom = comp.hotspots[i].bottom;
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
	g_engine_context.viewport->scrollx = sfc.scrollx;
	g_engine_context.viewport->scrolly = sfc.scrolly;
}
void SFCLoader::load_map() {
	if (sfc.map->unused_is_wrappable != 0) {
		fmt::print("WARN [SFCLoader] unused_is_wrappable = {}, expected 0\n", sfc.map->unused_is_wrappable);
	}
	fmt::print("WARN [SFCLoader] Unsupported: time_of_day = {}\n", sfc.map->time_of_day);

	fmt::print("INFO [SFCLoader] Loading background = {}.spr\n", sfc.map->background->filename);
	auto background = g_engine_context.map->background = g_engine_context.images->get_image(sfc.map->background->filename, ImageManager::IMAGE_SPR);
	// TODO: do any C1 metarooms have non-standard sizes?
	if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
	}

	fmt::print("INFO [SFCLoader] Loading rooms...\n");
	for (auto& r : sfc.map->rooms) {
		Room room;
		room.left = r.left;
		room.top = r.top;
		room.right = r.right;
		room.bottom = r.bottom;
		room.type = r.type;
		g_engine_context.map->rooms.push_back(room);
	}
	fmt::print("WARN [SFCLoader] Unsupported: ground_level\n");
	fmt::print("WARN [SFCLoader] Unsupported: bacteria\n");
}

void SFCLoader::vehicle_from_sfc(Object* obj, const sfc::VehicleV1& veh) {
	obj->vehicle_data = std::make_unique<VehicleData>();

	obj->vehicle_data->xvel = fixed24_8_t::from_raw(veh.xvel_times_256);
	obj->vehicle_data->yvel = fixed24_8_t::from_raw(veh.yvel_times_256);

	auto& obj_x = obj->compound_data->parts[0].renderable.x;
	auto& obj_y = obj->compound_data->parts[0].renderable.y;
	auto veh_x = fixed24_8_t::from_raw(veh.x_times_256);
	auto veh_y = fixed24_8_t::from_raw(veh.y_times_256);
	if (obj_x != veh_x || obj_y != veh_y) {
		fmt::print("INFO [SFCLoader] Object {} {} {} position {}, {} overridden by VehicleData {}, {}\n", obj->family, obj->genus, obj->species, obj_x, obj_y, veh_x, veh_y);
		obj_x = veh_x;
		obj_y = veh_y;
	}

	obj->vehicle_data->cabin_left = veh.cabin_left;
	obj->vehicle_data->cabin_top = veh.cabin_top;
	obj->vehicle_data->cabin_right = veh.cabin_right;
	obj->vehicle_data->cabin_bottom = veh.cabin_bottom;
	obj->vehicle_data->bump = veh.bump;
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
			if (dynamic_cast<sfc::CallButtonV1*>(p)) {
				fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type CallButton\n", obj->family, obj->genus, obj->species);
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
				if (dynamic_cast<sfc::LiftV1*>(p)) {
					fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Lift\n", obj->family, obj->genus, obj->species);
				}
			}
			if (dynamic_cast<sfc::BlackboardV1*>(p)) {
				fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Blackboard\n", obj->family, obj->genus, obj->species);
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