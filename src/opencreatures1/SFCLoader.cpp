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

struct SFCLoader {
	SFCLoader(const sfc::SFCFile& sfc_)
		: sfc(sfc_) {}
	void load_everything();

	void load_viewport();
	void load_map();
	void load_objects_and_sceneries();
	void load_object(const sfc::ObjectV1* p);
	Renderable renderable_from_sfc_entity(const sfc::EntityV1* part);
	void load_scripts();
	void load_macros();

	const sfc::SFCFile& sfc;
	std::map<const sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;
};

void SFCLoader::load_everything() {
	fmt::print("INFO [SFCLoader] Loading map...\n");
	load_map();

	fmt::print("INFO [SFCLoader] Loading objects and sceneries...\n");
	load_objects_and_sceneries();

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

	fmt::print("INFO [SFCLoader] Loading groundlevel...\n");
	g_engine_context.map->set_groundlevel(sfc.map->groundlevel);

	fmt::print("WARN [SFCLoader] Unsupported: bacteria\n");
}

Renderable SFCLoader::renderable_from_sfc_entity(const sfc::EntityV1* part) {
	if (part->x >= CREATURES1_WORLD_WIDTH) {
		throw Exception(fmt::format("Expected x to be between [0, {}), but got {}", CREATURES1_WORLD_WIDTH, part->x));
	}
	if (part->y >= CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected y to be between [0, {}), but got {}", CREATURES1_WORLD_HEIGHT, part->y));
	}

	Renderable r;
	r.set_position(part->x, part->y);
	r.set_z_order(part->z_order);
	r.set_object_sprite_base(part->sprite->first_sprite);
	r.set_part_sprite_base(part->image_offset);
	r.set_sprite_index(part->current_sprite - part->image_offset);
	r.set_sprite(g_engine_context.images->get_image(part->sprite->filename, ImageManager::IMAGE_SPR));
	if (part->has_animation) {
		r.set_animation(part->animation_frame, part->animation_string);
	}
	return r;
}

void SFCLoader::load_object(const sfc::ObjectV1* p) {
	// get the empty Object mapped to this sfc::ObjectV1
	auto handle = sfc_object_mapping[p];
	auto* obj = g_engine_context.objects->try_get(handle);
	obj->species = p->species;
	obj->genus = p->genus;
	obj->family = p->family;
	obj->movement_status = MovementStatus(p->movement_status);
	obj->attr = p->attr;
	obj->limit.x = p->limit_left;
	obj->limit.y = p->limit_top;
	obj->limit.width = p->limit_right - p->limit_left;
	obj->limit.height = p->limit_bottom - p->limit_top;
	obj->carrier = sfc_object_mapping[p->carrier];
	obj->actv = ActiveFlag(p->actv);
	// creaturesImage sprite;
	obj->tick_value = p->tick_value;
	obj->ticks_since_last_tick_event = p->ticks_since_last_tick_event;
	// Set the sound later, once we've loaded the object's position from its Entities
	// obj->current_sound = p->current_sound;
	obj->objp = sfc_object_mapping[p->objp];
	obj->obv0 = p->obv0;
	obj->obv1 = p->obv1;
	obj->obv2 = p->obv2;

	if (auto* scen = dynamic_cast<const sfc::SceneryV1*>(p)) {
		obj->scenery_data = std::make_unique<SceneryData>();
		obj->scenery_data->part = renderable_from_sfc_entity(scen->part);
	}

	if (auto* simp = dynamic_cast<const sfc::SimpleObjectV1*>(p)) {
		obj->simple_data = std::make_unique<SimpleObjectData>();
		obj->simple_data->part = renderable_from_sfc_entity(simp->part);
		obj->simple_data->z_order = simp->z_order;
		obj->simple_data->click_bhvr = simp->click_bhvr;
		obj->simple_data->touch_bhvr = simp->touch_bhvr;
	}

	if (dynamic_cast<const sfc::BubbleV1*>(p)) {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type: Bubble\n", obj->family, obj->genus, obj->species);
	}

	if (auto* cb = dynamic_cast<const sfc::CallButtonV1*>(p)) {
		obj->call_button_data = std::make_unique<CallButtonData>();
		obj->call_button_data->lift = sfc_object_mapping[cb->lift];
		obj->call_button_data->floor = cb->floor;
	}

	if (auto* pt = dynamic_cast<const sfc::PointerToolV1*>(p)) {
		obj->pointer_data = std::make_unique<PointerToolData>();
		obj->pointer_data->relx = pt->relx;
		obj->pointer_data->rely = pt->rely;
		obj->pointer_data->bubble = sfc_object_mapping[pt->bubble];
		obj->pointer_data->text = pt->text;
		g_engine_context.pointer->m_pointer_tool = handle;
	}

	if (auto* comp = dynamic_cast<const sfc::CompoundObjectV1*>(p)) {
		obj->compound_data = std::make_unique<CompoundObjectData>();
		for (auto& cp : comp->parts) {
			obj->compound_data->parts.emplace_back();
			obj->compound_data->parts.back().renderable = renderable_from_sfc_entity(cp.entity);
			obj->compound_data->parts.back().x = cp.x;
			obj->compound_data->parts.back().y = cp.y;
		}
		for (size_t i = 0; i < obj->compound_data->hotspots.size(); ++i) {
			obj->compound_data->hotspots[i].x = comp->hotspots[i].left;
			obj->compound_data->hotspots[i].y = comp->hotspots[i].top;
			obj->compound_data->hotspots[i].width = comp->hotspots[i].right - comp->hotspots[i].left;
			obj->compound_data->hotspots[i].height = comp->hotspots[i].bottom - comp->hotspots[i].top;
		}
		for (size_t i = 0; i < obj->compound_data->functions_to_hotspots.size(); ++i) {
			obj->compound_data->functions_to_hotspots[i] = comp->functions_to_hotspots[i];
		}
	}

	if (auto* veh = dynamic_cast<const sfc::VehicleV1*>(p)) {
		obj->vehicle_data = std::make_unique<VehicleData>();
		obj->vehicle_data->xvel = veh->xvel_times_256 / 256.f;
		obj->vehicle_data->yvel = veh->yvel_times_256 / 256.f;
		auto obj_x = obj->compound_data->parts[0].renderable.get_x();
		auto obj_y = obj->compound_data->parts[0].renderable.get_y();
		auto veh_x = veh->x_times_256 / 256.f;
		auto veh_y = veh->y_times_256 / 256.f;
		if (obj_x != veh_x || obj_y != veh_y) {
			fmt::print("INFO [SFCLoader] Object {} {} {} position {}, {} overridden by VehicleData {}, {}\n", obj->family, obj->genus, obj->species, obj_x, obj_y, veh_x, veh_y);
			obj->compound_data->parts[0].renderable.set_position(veh_x, veh_y);
			fmt::print("TODO: should we add the floating point bit to other parts' positions as well?\n");
		}
		obj->vehicle_data->cabin_left = veh->cabin_left;
		obj->vehicle_data->cabin_top = veh->cabin_top;
		obj->vehicle_data->cabin_right = veh->cabin_right;
		obj->vehicle_data->cabin_bottom = veh->cabin_bottom;
		obj->vehicle_data->bump = veh->bump;
	}

	if (auto* lift = dynamic_cast<const sfc::LiftV1*>(p)) {
		obj->lift_data = std::make_unique<LiftData>();
		obj->lift_data->next_or_current_floor = lift->next_or_current_floor;
		// TODO:
		// obj->lift_data->current_call_button = lift->current_call_button;
		fmt::print("WARN [SFCLoader] Unsupported: LiftData.current_call_button\n");
		// obj->lift_data->delay_counter = lift->delay_counter;
		fmt::print("WARN [SFCLoader] Unsupported: LiftData.delay_counter\n");
		for (size_t i = 0; i < numeric_cast<size_t>(lift->num_floors); ++i) {
			obj->lift_data->floors.push_back({lift->floors[i].y,
				sfc_object_mapping[lift->floors[i].call_button]});
		}
	}

	if (auto* bbd = dynamic_cast<const sfc::BlackboardV1*>(p)) {
		obj->blackboard_data = std::make_unique<BlackboardData>();
		obj->blackboard_data->background_color = bbd->background_color;
		obj->blackboard_data->chalk_color = bbd->chalk_color;
		obj->blackboard_data->alias_color = bbd->alias_color;
		obj->blackboard_data->text_x_position = bbd->text_x_position;
		obj->blackboard_data->text_y_position = bbd->text_y_position;
		for (size_t i = 0; i < bbd->words.size(); ++i) {
			auto& word = bbd->words[i];
			obj->blackboard_data->words[i].value = word.value;
			obj->blackboard_data->words[i].text = word.text;
		}
		obj->blackboard_data->charset_sprite = g_engine_context.images->get_charset_dta(bbd->background_color, bbd->chalk_color, bbd->alias_color);
	}

	if (dynamic_cast<const sfc::CreatureV1*>(p)) {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Creature\n", obj->family, obj->genus, obj->species);
	}

	// do this _after_ loading the SimpleObject or CompoundObject data,
	// since only then do we know the object's position / bounding box
	if (!p->current_sound.empty()) {
		// these won't be audible immediately, since the SoundManager thinks
		// they're out of hearing range. once the game starts and the
		// listener viewport gets set these will start being audible.
		obj->current_sound = g_engine_context.sounds->play_controlled_sound(p->current_sound, obj->get_bbox(), true);
	}
}

void SFCLoader::load_objects_and_sceneries() {
	// this part's a little tricky, since SFC objects can encode pointers to other
	// objects. we handle this by loading all objects empty first, and then do a
	// second pass where we actual fill in data — including remapping the pointers
	// to the newly created objects.
	// (as an alternative, we could try to do this in a depth-first style, where
	// loading an object also loads objects it has pointers to. we would still need
	// the mapping to fix up later pointers.)

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
		load_object(p);
	}
	fmt::print("INFO [SFCLoader] Loaded {} objects\n", sfc.objects.size());
	for (auto* p : sfc.sceneries) {
		load_object(p);
	}
	fmt::print("INFO [SFCLoader] Loaded {} sceneries\n", sfc.sceneries.size());
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

void sfc_load_everything(const sfc::SFCFile& sfc) {
	SFCLoader loader(sfc);
	loader.load_everything();
}