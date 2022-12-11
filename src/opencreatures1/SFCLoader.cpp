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

static Object object_without_refs_from_sfc(const sfc::ObjectV1& p) {
	Object obj;
	obj.species = p.species;
	obj.genus = p.genus;
	obj.family = p.family;
	obj.movement_status = MovementStatus(p.movement_status);
	obj.attr = p.attr;
	obj.limit.left = p.limit_left;
	obj.limit.top = p.limit_top;
	obj.limit.right = p.limit_right;
	obj.limit.bottom = p.limit_bottom;
	// obj.carrier = sfc_object_mapping.at(p.carrier);
	obj.actv = ActiveFlag(p.actv);
	// creaturesImage sprite;
	obj.tick_value = p.tick_value;
	obj.ticks_since_last_tick_event = p.ticks_since_last_tick_event;
	if (!p.current_sound.empty()) {
		obj.current_sound = g_engine_context.sounds->play_sound(p.current_sound, true);
	}
	// obj.objp = sfc_object_mapping.at(p.objp);
	obj.obv0 = p.obv0;
	obj.obv1 = p.obv1;
	obj.obv2 = p.obv2;
	return obj;
}

static SimpleObject simple_object_without_refs_from_sfc(const sfc::SimpleObjectV1& p) {
	SimpleObject obj;
	static_cast<Object&>(obj) = object_without_refs_from_sfc(p);
	obj.part = renderable_from_sfc_entity(*p.part);
	obj.z_order = p.z_order;
	obj.click_bhvr = p.click_bhvr;
	obj.touch_bhvr = p.touch_bhvr;
	return obj;
}

static CompoundObject compound_object_without_refs_from_sfc(const sfc::CompoundObjectV1& comp) {
	CompoundObject obj;
	static_cast<Object&>(obj) = object_without_refs_from_sfc(comp);

	for (auto& cp : comp.parts) {
		CompoundPart part;
		part.renderable = renderable_from_sfc_entity(*cp.entity);
		part.x = cp.x;
		part.y = cp.y;
		obj.parts.push_back(part);
	}
	for (size_t i = 0; i < obj.hotspots.size(); ++i) {
		obj.hotspots[i].left = comp.hotspots[i].left;
		obj.hotspots[i].top = comp.hotspots[i].top;
		obj.hotspots[i].right = comp.hotspots[i].right;
		obj.hotspots[i].bottom = comp.hotspots[i].bottom;
	}
	for (size_t i = 0; i < obj.functions_to_hotspots.size(); ++i) {
		obj.functions_to_hotspots[i] = comp.functions_to_hotspots[i];
	}

	return obj;
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
	// find our pointer
	for (auto obj : g_engine_context.objects->find_all<PointerTool>()) {
		g_engine_context.pointer->m_pointer_tool = obj;
	}

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

static Vehicle vehicle_without_refs_from_sfc(const sfc::VehicleV1& veh) {
	Vehicle obj;
	static_cast<CompoundObject&>(obj) = compound_object_without_refs_from_sfc(veh);

	obj.xvel = fixed24_8_t::from_raw(veh.xvel_times_256);
	obj.yvel = fixed24_8_t::from_raw(veh.yvel_times_256);
	// TODO: assert the high-precision position and low-precision position are equal?
	// obj.x_times_256 = veh.x_times_256;
	// obj.y_times_256 = veh.y_times_256;
	obj.cabin_left = veh.cabin_left;
	obj.cabin_top = veh.cabin_top;
	obj.cabin_right = veh.cabin_right;
	obj.cabin_bottom = veh.cabin_bottom;
	obj.bump = veh.bump;

	return obj;
}


void SFCLoader::load_objects() {
	// first load toplevel objects
	for (auto* p : sfc.objects) {
		if (dynamic_cast<sfc::BubbleV1*>(p)) {
			fmt::print("WARN [SFCLoader] Unsupported object type: Bubble\n");
		}
		if (dynamic_cast<sfc::CallButtonV1*>(p)) {
			fmt::print("WARN [SFCLoader] Unsupported object type: CallButton\n");
		}
		if (dynamic_cast<sfc::LiftV1*>(p)) {
			fmt::print("WARN [SFCLoader] Unsupported object type: Lift\n");
		}
		if (dynamic_cast<sfc::BlackboardV1*>(p)) {
			fmt::print("WARN [SFCLoader] Unsupported object type: Blackboard\n");
		}
		if (dynamic_cast<sfc::CreatureV1*>(p)) {
			fmt::print("WARN [SFCLoader] Unsupported object type: Creature\n");
		}

		if (auto* pt = dynamic_cast<sfc::PointerToolV1*>(p)) {
			PointerTool obj;
			static_cast<SimpleObject&>(obj) = simple_object_without_refs_from_sfc(*pt);
			obj.relx = pt->relx;
			obj.rely = pt->rely;
			// obj.bubble = pt->bubble;
			obj.text = pt->text;
			auto handle = g_engine_context.objects->add(obj);
			sfc_object_mapping[p] = handle;

		} else if (auto* simp = dynamic_cast<sfc::SimpleObjectV1*>(p)) {
			SimpleObject obj = simple_object_without_refs_from_sfc(*simp);
			sfc_object_mapping[p] = g_engine_context.objects->add(obj);

		} else if (auto* veh = dynamic_cast<sfc::VehicleV1*>(p)) {
			Vehicle obj = vehicle_without_refs_from_sfc(*veh);
			sfc_object_mapping[p] = g_engine_context.objects->add(obj);

		} else if (auto* comp = dynamic_cast<sfc::CompoundObjectV1*>(p)) {
			CompoundObject obj = compound_object_without_refs_from_sfc(*comp);
			sfc_object_mapping[p] = g_engine_context.objects->add(obj);

		} else {
			Object obj = object_without_refs_from_sfc(*p);
			sfc_object_mapping[p] = g_engine_context.objects->add(obj);
		}
	}
	for (auto* p : sfc.sceneries) {
		Scenery scen;
		static_cast<Object&>(scen) = object_without_refs_from_sfc(*p);
		scen.part = renderable_from_sfc_entity(*p->part);
		sfc_object_mapping[p] = g_engine_context.objects->add(scen);
	}
	// patch up object references
	for (auto* p : sfc.objects) {
		ObjectHandle handle = sfc_object_mapping[p];
		Object* obj = g_engine_context.objects->try_get<Object>(handle);
		obj->carrier = sfc_object_mapping[p->carrier];
		obj->objp = sfc_object_mapping[p->objp];

		if (auto* pt = dynamic_cast<sfc::PointerToolV1*>(p)) {
			auto pointertool = g_engine_context.objects->try_get<PointerTool>(handle);
			pointertool->bubble = sfc_object_mapping[pt->bubble];
		}
	}
	for (auto* p : sfc.sceneries) {
		Object* obj = g_engine_context.objects->try_get<Object>(sfc_object_mapping[p]);
		obj->carrier = sfc_object_mapping[p->carrier];
		obj->objp = sfc_object_mapping[p->objp];
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