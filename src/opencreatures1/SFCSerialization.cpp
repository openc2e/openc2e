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
		obj->scenery_data->part = renderable_from_sfc_entity(scen->part.get());
	}

	if (auto* simp = dynamic_cast<const sfc::SimpleObjectV1*>(p)) {
		obj->simple_data = std::make_unique<SimpleObjectData>();
		obj->simple_data->part = renderable_from_sfc_entity(simp->part.get());
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
			obj->compound_data->parts.back().renderable = renderable_from_sfc_entity(cp.entity.get());
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
		obj->lift_data->current_call_button = lift->current_call_button;
		// TODO
		// obj->lift_data->delay_ticks_divided_by_32 = lift->delay_ticks_divided_by_32;
		if (lift->delay_ticks_divided_by_32 != 0) {
			fmt::print("WARN [SFCLoader] Unsupported: LiftData.delay_ticks_divided_by_32 = {}\n", lift->delay_ticks_divided_by_32);
		}
		for (size_t i = 0; i < numeric_cast<size_t>(lift->num_floors); ++i) {
			// printf("lift->floors[i] y %i call_button %p\n", lift->floors[i].y, lift->floors[i].call_button);
			obj->lift_data->floors.push_back(lift->floors[i]);
		}
		for (size_t i = 0; i < lift->activated_call_buttons.size(); ++i) {
			obj->lift_data->activated_call_buttons[i] = sfc_object_mapping[lift->activated_call_buttons[i]];
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
	for (const auto& p : sfc.objects) {
		auto handle = g_engine_context.objects->add();
		sfc_object_mapping[p.get()] = handle;
	}
	for (const auto& p : sfc.sceneries) {
		auto handle = g_engine_context.objects->add();
		sfc_object_mapping[p.get()] = handle;
	}

	// second, load data, including cross-object references
	for (const auto& p : sfc.objects) {
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
	// so far, and convert ObjectHandles / copy them to the SFCFile on the fly.

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

		if (p->scenery_data) {
			auto scen = std::make_shared<sfc::SceneryV1>();
			sfc_object_mapping[p] = obj = scen;

			auto part = sfc_dump_entity(p->scenery_data->part);
			gallery = part->gallery;

			// SceneryV1
			scen->part = part;
		}

		else if (p->simple_data) {
			std::shared_ptr<sfc::SimpleObjectV1> simp;
			if (p->bubble_data) {
				fmt::print("WARN [SFCWriter] Unsupported type: Bubble\n");
			} else if (p->call_button_data) {
				auto cbtn = std::make_shared<sfc::CallButtonV1>();
				sfc_object_mapping[p] = obj = simp = cbtn;

				// CallButtonV1
				cbtn->lift = dynamic_cast<sfc::LiftV1*>(
					dump_object(g_engine_context.objects->try_get(p->call_button_data->lift)).get());
				cbtn->floor = p->call_button_data->floor;

			} else if (p->pointer_data) {
				auto pntr = std::make_shared<sfc::PointerToolV1>();
				sfc_object_mapping[p] = obj = simp = pntr;

				// PointerToolV1
				pntr->relx = p->pointer_data->relx;
				pntr->rely = p->pointer_data->rely;
				pntr->bubble = dynamic_cast<sfc::BubbleV1*>(
					dump_object(g_engine_context.objects->try_get(p->pointer_data->bubble)).get());
				pntr->text = p->pointer_data->text;
			}

			if (!simp) {
				simp = std::make_shared<sfc::SimpleObjectV1>();
				sfc_object_mapping[p] = obj = simp;
			}

			auto part = sfc_dump_entity(p->simple_data->part);
			gallery = part->gallery;

			//SimpleObjectV1
			simp->part = part;
			simp->z_order = p->simple_data->z_order;
			simp->click_bhvr = p->simple_data->click_bhvr;
			simp->touch_bhvr = p->simple_data->touch_bhvr;
		}

		else if (p->compound_data) {
			std::shared_ptr<sfc::CompoundObjectV1> comp;

			if (p->vehicle_data) {
				std::shared_ptr<sfc::VehicleV1> veh;

				if (p->lift_data) {
					auto lift = std::make_shared<sfc::LiftV1>();
					sfc_object_mapping[p] = obj = comp = veh = lift;

					// LiftV1
					lift->num_floors = numeric_cast<int32_t>(p->lift_data->floors.size());
					lift->next_or_current_floor = p->lift_data->next_or_current_floor;
					lift->current_call_button = p->lift_data->current_call_button;
					lift->delay_ticks_divided_by_32 = 0; // TODO
					for (size_t i = 0; i < p->lift_data->floors.size(); ++i) {
						lift->floors[i] = p->lift_data->floors[i];
					}
					for (size_t i = 0; i < p->lift_data->activated_call_buttons.size(); ++i) {
						lift->activated_call_buttons[i] = dynamic_cast<sfc::CallButtonV1*>(
							dump_object(g_engine_context.objects->try_get(p->lift_data->activated_call_buttons[i])).get());
					}
				}

				if (!veh) {
					veh = std::make_shared<sfc::VehicleV1>();
					sfc_object_mapping[p] = obj = comp = veh;
				}
				// VehicleV1
				// TODO: round or truncate velocity? does it matter?
				veh->x_times_256 = numeric_cast<int32_t>(p->compound_data->parts[0].renderable.get_x() * 256);
				veh->y_times_256 = numeric_cast<int32_t>(p->compound_data->parts[0].renderable.get_y() * 256);
				veh->xvel_times_256 = numeric_cast<int32_t>(p->vehicle_data->xvel * 256);
				veh->yvel_times_256 = numeric_cast<int32_t>(p->vehicle_data->yvel * 256);
				veh->cabin_left = p->vehicle_data->cabin_left;
				veh->cabin_top = p->vehicle_data->cabin_top;
				veh->cabin_right = p->vehicle_data->cabin_right;
				veh->cabin_bottom = p->vehicle_data->cabin_bottom;
				veh->bump = p->vehicle_data->bump;

			} else if (p->blackboard_data) {
				auto bbd = std::make_shared<sfc::BlackboardV1>();
				sfc_object_mapping[p] = obj = comp = bbd;

				// BlackboardV1
				bbd->background_color = p->blackboard_data->background_color;
				bbd->chalk_color = p->blackboard_data->chalk_color;
				bbd->alias_color = p->blackboard_data->alias_color;
				bbd->text_x_position = p->blackboard_data->text_x_position;
				bbd->text_y_position = p->blackboard_data->text_y_position;
				for (size_t i = 0; i < bbd->words.size(); ++i) {
					bbd->words[i].value = p->blackboard_data->words[i].value;
					bbd->words[i].text = p->blackboard_data->words[i].text;
				}
			}

			if (!comp) {
				comp = std::make_shared<sfc::CompoundObjectV1>();
				sfc_object_mapping[p] = obj = comp;
			}

			// CompoundObjectV1
			for (auto& part : p->compound_data->parts) {
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
				comp->hotspots[i].left = p->compound_data->hotspots[i].x;
				comp->hotspots[i].top = p->compound_data->hotspots[i].y;
				comp->hotspots[i].right = p->compound_data->hotspots[i].right();
				comp->hotspots[i].bottom = p->compound_data->hotspots[i].bottom();
			}
			comp->functions_to_hotspots = p->compound_data->functions_to_hotspots;
		}

		else if (p->creature_data) {
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

		if (p->scenery_data) {
			sfc.sceneries.push_back(std::dynamic_pointer_cast<sfc::SceneryV1>(obj));
		} else {
			sfc.objects.push_back(obj);
		}
		return obj;
	};

	fmt::print("INFO [SFCWriter] Writing objects and sceneries...\n");
	for (auto& p : *g_engine_context.objects) {
		dump_object(p.get());
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