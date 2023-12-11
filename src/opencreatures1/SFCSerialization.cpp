#include "SFCSerialization.h"

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
#include "common/Ranges.h"

#include <fmt/ranges.h>

struct SFCLoader {
	SFCLoader(const sfc::SFCFile& sfc_)
		: sfc(sfc_) {}
	void load_everything();

	void load_viewport();
	void load_map();
	void load_objects_and_sceneries();
	ObjectHandle load_object(const sfc::ObjectV1* p);
	Renderable renderable_from_sfc_entity(const sfc::EntityV1* part);
	void load_scripts();
	void load_macros();

	void load_object(const sfc::ObjectV1*, Object*);
	void load_object(const sfc::SceneryV1*, Scenery*);
	void load_object(const sfc::SimpleObjectV1*, SimpleObject*);
	void load_object(const sfc::BubbleV1*, Bubble*);
	void load_object(const sfc::CallButtonV1*, CallButton*);
	void load_object(const sfc::PointerToolV1*, PointerTool*);
	void load_object(const sfc::CompoundObjectV1*, CompoundObject*);
	void load_object(const sfc::VehicleV1*, Vehicle*);
	void load_object(const sfc::LiftV1*, Lift*);
	void load_object(const sfc::BlackboardV1*, Blackboard*);
	void load_object(const sfc::CreatureV1*, Creature*);

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
	fmt::print("INFO [SFCLoader] Viewport {}, {}\n", sfc.scrollx, sfc.scrolly);
	g_engine_context.viewport->set_scroll_position(sfc.scrollx, sfc.scrolly);
}

void SFCLoader::load_map() {
	if (sfc.map->unused_is_wrappable != 0) {
		fmt::print("WARN [SFCLoader] unused_is_wrappable = {}, expected 0\n", sfc.map->unused_is_wrappable);
	}
	fmt::print("WARN [SFCLoader] Unsupported: time_of_day = {}\n", sfc.map->time_of_day);

	fmt::print("INFO [SFCLoader] Loading background = {}.spr\n", sfc.map->background->filename);
	auto background = g_engine_context.images->get_image(
		sfc.map->background->filename,
		sfc.map->background->absolute_base,
		numeric_cast<int32_t>(sfc.map->background->images.size()),
		static_cast<ImageManager::ImageType>(ImageManager::IMAGE_SPR | ImageManager::IMAGE_IS_BACKGROUND));
	if (sfc.map->background->absolute_base != 0) {
		fmt::print("WARN [SFCLoader] map background absolute base = {}, expected 0", sfc.map->background->absolute_base);
	}
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

static std::shared_ptr<sfc::MapDataV1> sfc_dump_map() {
	auto map = std::make_shared<sfc::MapDataV1>();
	map->background = std::make_shared<sfc::CGalleryV1>();
	map->background->filename = g_engine_context.map->get_background().name;
	map->background->absolute_base = g_engine_context.map->get_background().absolute_base;
	map->background->refcount = 1;

	// don't use sfc_dump_gallery, because map galleries are stitched together unlike normal sprites.
	// we hardcode the tile sizes here since they're always the same, instead of storing the original
	// sizes on the gallery.
	for (auto offset : g_engine_context.map->get_background().offsets) {
		map->background->images.emplace_back();
		auto& image = map->background->images.back();
		image.parent = map->background.get();
		image.status = 0; // seems to work???
		image.width = 144;
		image.height = 150;
		image.offset = offset;
	}

	fmt::print("INFO [SFCWriter] Writing rooms...\n");
	for (auto& r : g_engine_context.map->get_rooms()) {
		sfc::RoomV1 room;
		room.left = r.left;
		room.top = r.top;
		room.right = r.right;
		room.bottom = r.bottom;
		room.type = r.type;
		map->rooms.push_back(room);
	}

	fmt::print("INFO [SFCWriter] Writing groundlevel...\n");
	map->groundlevel = g_engine_context.map->get_groundlevel();

	return map;
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
	r.set_base(part->sprite_base);
	r.set_pose(part->sprite_pose_plus_base - part->sprite_base);
	r.set_gallery(g_engine_context.images->get_image(
		part->gallery->filename,
		part->gallery->absolute_base,
		numeric_cast<int32_t>(part->gallery->images.size()),
		ImageManager::IMAGE_SPR));
	if (part->has_animation) {
		r.set_animation(part->animation_frame, part->animation_string);
	}
	return r;
}

void SFCLoader::load_object(const sfc::ObjectV1* p, Object* obj) {
	obj->species = p->species;
	obj->genus = p->genus;
	obj->family = p->family;
	obj->movement_status = MovementStatus(p->movement_status);
	obj->attr = p->attr;
	obj->limit.x = p->limit_left;
	obj->limit.y = p->limit_top;
	obj->limit.width = p->limit_right - p->limit_left;
	obj->limit.height = p->limit_bottom - p->limit_top;
	obj->carrier = load_object(p->carrier);
	obj->actv = ActiveFlag(p->actv);
	// creaturesImage sprite;
	obj->tick_value = p->tick_value;
	obj->ticks_since_last_tick_event = p->ticks_since_last_tick_event;
	// Set the sound later, once we've loaded the object's position from its Entities
	// obj->current_sound = p->current_sound;
	obj->objp = load_object(p->objp);
	obj->obv0 = p->obv0;
	obj->obv1 = p->obv1;
	obj->obv2 = p->obv2;
}

void SFCLoader::load_object(const sfc::SceneryV1* scen, Scenery* obj) {
	load_object(static_cast<const sfc::ObjectV1*>(scen), static_cast<Object*>(obj));
	obj->part = renderable_from_sfc_entity(scen->part.get());
}

void SFCLoader::load_object(const sfc::SimpleObjectV1* simp, SimpleObject* obj) {
	load_object(static_cast<const sfc::ObjectV1*>(simp), static_cast<Object*>(obj));
	obj->part = renderable_from_sfc_entity(simp->part.get());
	obj->z_order = simp->z_order;
	obj->click_bhvr = simp->click_bhvr;
	obj->touch_bhvr = simp->touch_bhvr;
}

void SFCLoader::load_object(const sfc::BubbleV1* bub, Bubble* obj) {
	load_object(static_cast<const sfc::SimpleObjectV1*>(bub), static_cast<SimpleObject*>(obj));
	fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type: Bubble\n", obj->family, obj->genus, obj->species);
}

void SFCLoader::load_object(const sfc::CallButtonV1* cb, CallButton* obj) {
	load_object(static_cast<const sfc::SimpleObjectV1*>(cb), static_cast<SimpleObject*>(obj));
	obj->lift = load_object(cb->lift);
	obj->floor = cb->floor;
}

void SFCLoader::load_object(const sfc::PointerToolV1* pt, PointerTool* obj) {
	load_object(static_cast<const sfc::SimpleObjectV1*>(pt), static_cast<SimpleObject*>(obj));
	obj->relx = pt->relx;
	obj->rely = pt->rely;
	obj->bubble = load_object(pt->bubble);
	obj->text = pt->text;
	g_engine_context.pointer->m_pointer_tool = obj->uid;
}

void SFCLoader::load_object(const sfc::CompoundObjectV1* comp, CompoundObject* obj) {
	load_object(static_cast<const sfc::ObjectV1*>(comp), static_cast<Object*>(obj));

	for (auto& cp : comp->parts) {
		obj->parts.emplace_back();
		obj->parts.back().renderable = renderable_from_sfc_entity(cp.entity.get());
		obj->parts.back().x = cp.x;
		obj->parts.back().y = cp.y;
	}
	for (size_t i = 0; i < obj->hotspots.size(); ++i) {
		obj->hotspots[i].x = comp->hotspots[i].left;
		obj->hotspots[i].y = comp->hotspots[i].top;
		obj->hotspots[i].width = comp->hotspots[i].right - comp->hotspots[i].left;
		obj->hotspots[i].height = comp->hotspots[i].bottom - comp->hotspots[i].top;
	}
	for (size_t i = 0; i < obj->functions_to_hotspots.size(); ++i) {
		obj->functions_to_hotspots[i] = comp->functions_to_hotspots[i];
	}
}

void SFCLoader::load_object(const sfc::VehicleV1* veh, Vehicle* obj) {
	load_object(static_cast<const sfc::CompoundObjectV1*>(veh), static_cast<CompoundObject*>(obj));

	// need the compound parts to already be defined
	obj->xvel = veh->xvel_times_256 / 256.f;
	obj->yvel = veh->yvel_times_256 / 256.f;
	auto obj_x = obj->parts[0].renderable.get_x();
	auto obj_y = obj->parts[0].renderable.get_y();
	auto veh_x = veh->x_times_256 / 256.f;
	auto veh_y = veh->y_times_256 / 256.f;
	if (obj_x != veh_x || obj_y != veh_y) {
		fmt::print("INFO [SFCLoader] Object {} {} {} position {}, {} adjusted for VehicleData {}, {}\n", obj->family, obj->genus, obj->species, obj_x, obj_y, veh_x, veh_y);
		auto diff_x = veh_x - obj_x;
		auto diff_y = veh_y - obj_y;
		// All parts need to be adjusted, not just the main one. If only the main one
		// changes, things start to get weird — look at the cable car buttons after
		// multiple load/save cycles whilst activating the buttons. They seem to "shimmy"
		// up and down (classic floating point issue) and eventually drift after enough
		// serialization cycles.
		for (auto& cp : obj->parts) {
			cp.renderable.set_position(cp.renderable.get_x() + diff_x, cp.renderable.get_y() + diff_y);
		}
	}
	obj->cabin_left = veh->cabin_left;
	obj->cabin_top = veh->cabin_top;
	obj->cabin_right = veh->cabin_right;
	obj->cabin_bottom = veh->cabin_bottom;
	obj->bump = veh->bump;
}

void SFCLoader::load_object(const sfc::LiftV1* lift, Lift* obj) {
	load_object(static_cast<const sfc::VehicleV1*>(lift), static_cast<Vehicle*>(obj));

	obj->next_or_current_floor = lift->next_or_current_floor;
	// unneeded
	// obj->current_call_button = lift->current_call_button;
	// TODO
	// obj->delay_ticks_divided_by_32 = lift->delay_ticks_divided_by_32;
	if (lift->delay_ticks_divided_by_32 != 0) {
		fmt::print("WARN [SFCLoader] Unsupported: LiftData.delay_ticks_divided_by_32 = {}\n", lift->delay_ticks_divided_by_32);
	}
	for (size_t i = 0; i < numeric_cast<size_t>(lift->num_floors); ++i) {
		// printf("lift->floors[i] y %i call_button %p\n", lift->floors[i].y, lift->floors[i].call_button);
		obj->floors.push_back(lift->floors[i]);
	}
	for (auto* cb : lift->activated_call_buttons) {
		if (auto&& handle = load_object(cb)) {
			obj->activated_call_buttons.insert(handle);
		}
	}
}

void SFCLoader::load_object(const sfc::BlackboardV1* bbd, Blackboard* obj) {
	load_object(static_cast<const sfc::CompoundObjectV1*>(bbd), static_cast<CompoundObject*>(obj));

	obj->background_color = bbd->background_color;
	obj->chalk_color = bbd->chalk_color;
	obj->alias_color = bbd->alias_color;
	obj->text_x_position = bbd->text_x_position;
	obj->text_y_position = bbd->text_y_position;
	for (size_t i = 0; i < bbd->words.size(); ++i) {
		auto& word = bbd->words[i];
		obj->words[i].value = word.value;
		obj->words[i].text = word.text;
	}
	obj->charset_sprite = g_engine_context.images->get_charset_dta(bbd->background_color, bbd->chalk_color, bbd->alias_color);
}

void SFCLoader::load_object(const sfc::CreatureV1*, Creature* obj) {
	fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Creature\n", obj->family, obj->genus, obj->species);
}

ObjectHandle SFCLoader::load_object(const sfc::ObjectV1* p) {
	// null pointer?
	if (p == nullptr) {
		return {};
	}

	// has it been created yet?
	auto it = sfc_object_mapping.find(p);
	if (it != sfc_object_mapping.end()) {
		return it->second;
	}

	// do the type mapping game
	// TODO: ugh
	ObjectHandle handle;
	auto&& sfc_type = typeid(*p);
	if (sfc_type == typeid(sfc::SceneryV1)) {
		handle = g_engine_context.objects->add<Scenery>();
	} else if (sfc_type == typeid(sfc::SimpleObjectV1)) {
		handle = g_engine_context.objects->add<SimpleObject>();
	} else if (sfc_type == typeid(sfc::BubbleV1)) {
		handle = g_engine_context.objects->add<Bubble>();
	} else if (sfc_type == typeid(sfc::CallButtonV1)) {
		handle = g_engine_context.objects->add<CallButton>();
	} else if (sfc_type == typeid(sfc::PointerToolV1)) {
		handle = g_engine_context.objects->add<PointerTool>();
	} else if (sfc_type == typeid(sfc::CompoundObjectV1)) {
		handle = g_engine_context.objects->add<CompoundObject>();
	} else if (sfc_type == typeid(sfc::VehicleV1)) {
		handle = g_engine_context.objects->add<Vehicle>();
	} else if (sfc_type == typeid(sfc::LiftV1)) {
		handle = g_engine_context.objects->add<Lift>();
	} else if (sfc_type == typeid(sfc::BlackboardV1)) {
		handle = g_engine_context.objects->add<Blackboard>();
	} else if (sfc_type == typeid(sfc::CreatureV1)) {
		handle = g_engine_context.objects->add<Creature>();
	} else {
		throw Exception("Unknown object type");
	}

	sfc_object_mapping[p] = handle;
	auto* obj = g_engine_context.objects->try_get(handle);

	if (sfc_type == typeid(sfc::SceneryV1)) {
		load_object(static_cast<const sfc::SceneryV1*>(p), static_cast<Scenery*>(obj));
	} else if (sfc_type == typeid(sfc::SimpleObjectV1)) {
		load_object(static_cast<const sfc::SimpleObjectV1*>(p), static_cast<SimpleObject*>(obj));
	} else if (sfc_type == typeid(sfc::BubbleV1)) {
		load_object(static_cast<const sfc::BubbleV1*>(p), static_cast<Bubble*>(obj));
	} else if (sfc_type == typeid(sfc::CallButtonV1)) {
		load_object(static_cast<const sfc::CallButtonV1*>(p), static_cast<CallButton*>(obj));
	} else if (sfc_type == typeid(sfc::PointerToolV1)) {
		load_object(static_cast<const sfc::PointerToolV1*>(p), static_cast<PointerTool*>(obj));
	} else if (sfc_type == typeid(sfc::CompoundObjectV1)) {
		load_object(static_cast<const sfc::CompoundObjectV1*>(p), static_cast<CompoundObject*>(obj));
	} else if (sfc_type == typeid(sfc::VehicleV1)) {
		load_object(static_cast<const sfc::VehicleV1*>(p), static_cast<Vehicle*>(obj));
	} else if (sfc_type == typeid(sfc::LiftV1)) {
		load_object(static_cast<const sfc::LiftV1*>(p), static_cast<Lift*>(obj));
	} else if (sfc_type == typeid(sfc::BlackboardV1)) {
		load_object(static_cast<const sfc::BlackboardV1*>(p), static_cast<Blackboard*>(obj));
	} else if (sfc_type == typeid(sfc::CreatureV1)) {
		load_object(static_cast<const sfc::CreatureV1*>(p), static_cast<Creature*>(obj));
	} else {
		throw Exception("Unknown object type");
	}

	// do this _after_ loading the SimpleObject or CompoundObject data,
	// since only then do we know the object's position / bounding box
	// TODO: should probably somewhere else? like global initialization after everything's been loaded?
	if (!p->current_sound.empty()) {
		// these won't be audible immediately, since the SoundManager thinks
		// they're out of hearing range. once the game starts and the
		// listener viewport gets set these will start being audible.
		obj->current_sound = g_engine_context.sounds->play_controlled_sound(p->current_sound, obj->get_bbox(), true);
	}

	return handle;
}

void SFCLoader::load_objects_and_sceneries() {
	// this part's a little tricky, since SFC objects can encode pointers to other
	// objects. we keep a map of all sfc::ObjectV1* pointers we've seen so far, and
	// convert new pointers / load actual Objects on the fly. (note that references
	// can be cyclical, so the map must also keep a list of all pointers _in the process_
	// of being loaded.)
	for (const auto& p : sfc.objects) {
		auto sfc_object = p.get();
		auto&& sfc_type = typeid(*sfc_object);
		if (sfc_type == typeid(sfc::CreatureV1)) {
			continue;
		}
		load_object(p.get());
	}
	fmt::print("INFO [SFCLoader] Loaded {} objects\n", sfc.objects.size());
	for (const auto& p : sfc.sceneries) {
		load_object(p.get());
	}
	fmt::print("INFO [SFCLoader] Loaded {} sceneries\n", sfc.sceneries.size());
}

void SFCLoader::load_scripts() {
	for (auto& s : sfc.scripts) {
		g_engine_context.scriptorium->add(s.family, s.genus, s.species, s.eventno, s.text);
	}
}

void sfc_dump_scripts(sfc::SFCFile& sfc) {
	for (auto& p : *g_engine_context.scriptorium) {
		sfc::ScriptV1 script;
		script.family = p.first.family;
		script.genus = p.first.genus;
		script.species = p.first.species;
		script.eventno = p.first.eventno;
		script.text = p.second;
		sfc.scripts.push_back(script);
	}
}

void SFCLoader::load_macros() {
	for (const auto& m : sfc.macros) {
		Macro macro;
		macro.selfdestruct = m->selfdestruct;
		macro.inst = m->inst;
		macro.script = m->script;
		macro.ip = m->ip;
		for (size_t i = 0; i < m->sp; ++i) {
			macro.stack.push_back(m->stack[i]);
		}
		macro.vars = m->vars;
		macro.ownr = load_object(m->ownr);
		macro.from = load_object(m->from);
		macro.exec = load_object(m->exec);
		macro.targ = load_object(m->targ);
		macro._it_ = load_object(m->_it_);
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

static std::shared_ptr<sfc::CGalleryV1> sfc_dump_gallery(const ImageGallery& gallery) {
	auto sfc = std::make_shared<sfc::CGalleryV1>();
	sfc->filename = gallery.name;
	sfc->absolute_base = gallery.absolute_base;
	sfc->refcount = 1; // TODO
	sfc->images.resize(numeric_cast<size_t>(gallery.size()));

	for (size_t i = 0; i < sfc->images.size(); ++i) {
		auto& img = sfc->images[i];
		img.parent = sfc.get();
		img.status = 0; // seems to work???
		img.width = gallery.width(numeric_cast<int32_t>(i));
		img.height = gallery.height(numeric_cast<int32_t>(i));
		img.offset = gallery.offsets[i];
	}
	return sfc;
}

static std::shared_ptr<sfc::EntityV1> sfc_dump_entity(const Renderable& r) {
	// TODO: gallery should at least be shared between all parts on an object?
	auto gallery = sfc_dump_gallery(r.get_gallery());

	auto entity = std::make_shared<sfc::EntityV1>();
	entity->gallery = gallery;
	entity->sprite_pose_plus_base = numeric_cast<uint8_t>(r.get_pose() + r.get_base());
	entity->sprite_base = numeric_cast<uint8_t>(r.get_base());
	entity->z_order = r.get_z_order();
	entity->x = numeric_cast<int32_t>(r.get_x());
	entity->y = numeric_cast<int32_t>(r.get_y());
	entity->has_animation = r.has_animation();
	if (entity->has_animation) {
		entity->animation_frame = r.get_animation_frame(); // only if has_animation is true
		entity->animation_string = r.get_animation_string();
	}

	return entity;
}

static void sfc_dump_objects_and_sceneries_and_macros(sfc::SFCFile& sfc) {
	// so, similarly to _loading_ from the SFC, this is tricky since SFC objects
	// can encode points to other objects. we keep a map of all ObjectHandles we've
	// seen so far, and convert ObjectHandles / copy them to the SFCFile on the fly.

	std::map<Object*, std::shared_ptr<sfc::ObjectV1>> sfc_object_mapping;
	std::function<std::shared_ptr<sfc::ObjectV1>(Object*)> dump_object;
	dump_object = [&](Object* p) -> std::shared_ptr<sfc::ObjectV1> {
		if (!p) {
			return nullptr;
		}

		auto it = sfc_object_mapping.find(p);
		if (it != sfc_object_mapping.end()) {
			// already processed/processing this one
			return it->second;
		}

		std::shared_ptr<sfc::ObjectV1> obj;
		std::shared_ptr<sfc::CGalleryV1> gallery;

		if (p->as_scenery()) {
			auto scen = std::make_shared<sfc::SceneryV1>();
			sfc_object_mapping[p] = obj = scen;

			auto part = sfc_dump_entity(p->as_scenery()->part);
			gallery = part->gallery;

			// SceneryV1
			scen->part = part;
		}

		else if (p->as_simple_object()) {
			std::shared_ptr<sfc::SimpleObjectV1> simp;
			if (p->as_bubble()) {
				fmt::print("WARN [SFCWriter] Unsupported type: Bubble\n");
			} else if (p->as_call_button()) {
				auto cbtn = std::make_shared<sfc::CallButtonV1>();
				sfc_object_mapping[p] = obj = simp = cbtn;

				// CallButtonV1
				cbtn->lift = dynamic_cast<sfc::LiftV1*>(
					dump_object(g_engine_context.objects->try_get(p->as_call_button()->lift)).get());
				cbtn->floor = p->as_call_button()->floor;

			} else if (p->as_pointer_tool()) {
				auto pntr = std::make_shared<sfc::PointerToolV1>();
				sfc_object_mapping[p] = obj = simp = pntr;

				// PointerToolV1
				pntr->relx = p->as_pointer_tool()->relx;
				pntr->rely = p->as_pointer_tool()->rely;
				pntr->bubble = dynamic_cast<sfc::BubbleV1*>(
					dump_object(g_engine_context.objects->try_get(p->as_pointer_tool()->bubble)).get());
				pntr->text = p->as_pointer_tool()->text;
			}

			if (!simp) {
				simp = std::make_shared<sfc::SimpleObjectV1>();
				sfc_object_mapping[p] = obj = simp;
			}

			auto part = sfc_dump_entity(p->as_simple_object()->part);
			gallery = part->gallery;

			//SimpleObjectV1
			simp->part = part;
			simp->z_order = p->as_simple_object()->z_order;
			simp->click_bhvr = p->as_simple_object()->click_bhvr;
			simp->touch_bhvr = p->as_simple_object()->touch_bhvr;
		}

		else if (p->as_compound_object()) {
			std::shared_ptr<sfc::CompoundObjectV1> comp;

			if (p->as_vehicle()) {
				std::shared_ptr<sfc::VehicleV1> veh;

				if (p->as_lift()) {
					auto lift = std::make_shared<sfc::LiftV1>();
					sfc_object_mapping[p] = obj = comp = veh = lift;

					// LiftV1
					lift->num_floors = numeric_cast<int32_t>(p->as_lift()->floors.size());
					lift->next_or_current_floor = p->as_lift()->next_or_current_floor;

					// Would like to do this based on index w/in the serialized array, but
					// we're not guaranteed to have the CallButtons serialized by this point...
					// Since CallButtons have a pointer to their parent Lift, we might start
					// serializing a CallButton and immediately switch to serializing the Lift,
					// without fleshing out the CallButton struct until the Lift is finished.
					lift->current_call_button = index_if(p->as_lift()->activated_call_buttons, [&](auto& handle) {
						auto* cb = g_engine_context.objects->try_get(handle);
						return cb && cb->as_call_button()->floor == lift->next_or_current_floor;
					});

					lift->delay_ticks_divided_by_32 = 0; // TODO
					for (size_t i = 0; i < p->as_lift()->floors.size(); ++i) {
						lift->floors[i] = p->as_lift()->floors[i];
					}
					for (auto it : enumerate(p->as_lift()->activated_call_buttons)) {
						lift->activated_call_buttons[it.first] = dynamic_cast<sfc::CallButtonV1*>(
							dump_object(g_engine_context.objects->try_get(it.second)).get());
					}
				}

				if (!veh) {
					veh = std::make_shared<sfc::VehicleV1>();
					sfc_object_mapping[p] = obj = comp = veh;
				}
				// VehicleV1
				// TODO: round or truncate velocity? does it matter?
				veh->x_times_256 = numeric_cast<int32_t>(p->as_compound_object()->parts[0].renderable.get_x() * 256);
				veh->y_times_256 = numeric_cast<int32_t>(p->as_compound_object()->parts[0].renderable.get_y() * 256);
				veh->xvel_times_256 = numeric_cast<int32_t>(p->as_vehicle()->xvel * 256);
				veh->yvel_times_256 = numeric_cast<int32_t>(p->as_vehicle()->yvel * 256);
				veh->cabin_left = p->as_vehicle()->cabin_left;
				veh->cabin_top = p->as_vehicle()->cabin_top;
				veh->cabin_right = p->as_vehicle()->cabin_right;
				veh->cabin_bottom = p->as_vehicle()->cabin_bottom;
				veh->bump = p->as_vehicle()->bump;

			} else if (p->as_blackboard()) {
				auto bbd = std::make_shared<sfc::BlackboardV1>();
				sfc_object_mapping[p] = obj = comp = bbd;

				// BlackboardV1
				bbd->background_color = p->as_blackboard()->background_color;
				bbd->chalk_color = p->as_blackboard()->chalk_color;
				bbd->alias_color = p->as_blackboard()->alias_color;
				bbd->text_x_position = p->as_blackboard()->text_x_position;
				bbd->text_y_position = p->as_blackboard()->text_y_position;
				for (size_t i = 0; i < bbd->words.size(); ++i) {
					bbd->words[i].value = p->as_blackboard()->words[i].value;
					bbd->words[i].text = p->as_blackboard()->words[i].text;
				}
			}

			if (!comp) {
				comp = std::make_shared<sfc::CompoundObjectV1>();
				sfc_object_mapping[p] = obj = comp;
			}

			// CompoundObjectV1
			for (auto& part : p->as_compound_object()->parts) {
				sfc::CompoundPartV1 sfcpart;
				sfcpart.entity = sfc_dump_entity(part.renderable);
				if (!gallery) {
					gallery = sfcpart.entity->gallery;
				}
				sfcpart.x = part.x;
				sfcpart.y = part.y;
				comp->parts.push_back(sfcpart);
			}
			for (size_t i = 0; i < comp->hotspots.size(); ++i) {
				comp->hotspots[i].left = p->as_compound_object()->hotspots[i].x;
				comp->hotspots[i].top = p->as_compound_object()->hotspots[i].y;
				comp->hotspots[i].right = p->as_compound_object()->hotspots[i].right();
				comp->hotspots[i].bottom = p->as_compound_object()->hotspots[i].bottom();
			}
			comp->functions_to_hotspots = p->as_compound_object()->functions_to_hotspots;
		}

		else if (p->as_creature()) {
			fmt::print("WARN [SFCWriter] Unsupported type: Creature\n");
			return nullptr;
		}

		// ObjectV1
		obj->species = p->species;
		obj->genus = p->genus;
		obj->family = p->family;
		obj->movement_status = p->movement_status;
		obj->attr = p->attr;
		obj->limit_left = p->limit.x;
		obj->limit_top = p->limit.y;
		obj->limit_right = p->limit.right();
		obj->limit_bottom = p->limit.bottom();
		obj->carrier = dump_object(g_engine_context.objects->try_get(p->carrier)).get();
		obj->actv = p->actv;
		obj->gallery = gallery;
		obj->tick_value = p->tick_value;
		obj->ticks_since_last_tick_event = p->ticks_since_last_tick_event;
		obj->objp = dump_object(g_engine_context.objects->try_get(p->objp)).get();
		if (p->current_sound.get_looping()) {
			obj->current_sound = p->current_sound.get_name();
		}
		obj->obv0 = p->obv0;
		obj->obv1 = p->obv1;
		obj->obv2 = p->obv2;
		// TODO: I think we can skip scripts here, since they'll be loaded from the global
		// scriptorium anyways?
		// obj->scripts = p->scripts;

		if (p->as_scenery()) {
			sfc.sceneries.push_back(std::dynamic_pointer_cast<sfc::SceneryV1>(obj));
		} else {
			sfc.objects.push_back(obj);
		}
		return obj;
	};

	fmt::print("INFO [SFCWriter] Writing objects and sceneries...\n");
	for (auto* p : *g_engine_context.objects) {
		dump_object(p);
	}

	fmt::print("INFO [SFCWriter] Writing macros...\n");
	for (auto& m : *g_engine_context.macros) {
		auto mac = std::make_shared<sfc::MacroV1>();
		mac->selfdestruct = m.selfdestruct;
		mac->inst = m.inst;
		mac->script = m.script;
		mac->ip = m.ip;
		for (size_t i = 0; i < m.stack.size(); ++i) {
			mac->stack[i] = m.stack[i];
		}
		mac->sp = numeric_cast<uint32_t>(m.stack.size());
		mac->vars = m.vars;
		mac->ownr = dump_object(g_engine_context.objects->try_get(m.ownr)).get();
		mac->from = dump_object(g_engine_context.objects->try_get(m.from)).get();
		mac->exec = dump_object(g_engine_context.objects->try_get(m.exec)).get();
		mac->targ = dump_object(g_engine_context.objects->try_get(m.targ)).get();
		mac->_it_ = dump_object(g_engine_context.objects->try_get(m._it_)).get();
		mac->part = m.part;
		mac->subroutine_label = m.subroutine_label;
		mac->subroutine_address = m.subroutine_address;
		mac->wait = m.wait;

		if (m.destroy_as_soon_as_possible) {
			printf("WARN [SFCWriter] Macro.destroy_as_soon_as_possible unsupported\n");
		}
		if (m.enum_result.size()) {
			printf("ERROR [SFCWriter] Macro.enum_result unsupported\n");
		}

		sfc.macros.push_back(mac);
	}
}

sfc::SFCFile sfc_dump_everything() {
	sfc::SFCFile sfc;
	fmt::print("INFO [SFCWriter] Writing map...\n");
	sfc.map = sfc_dump_map();

	sfc_dump_objects_and_sceneries_and_macros(sfc);

	fmt::print("INFO [SFCWriter] Writing scriptorium...\n");
	sfc_dump_scripts(sfc);

	fmt::print("INFO [SFCWriter] Writing viewport...\n");
	sfc.scrollx = g_engine_context.viewport->get_scrollx();
	sfc.scrolly = g_engine_context.viewport->get_scrolly();

	return sfc;
}