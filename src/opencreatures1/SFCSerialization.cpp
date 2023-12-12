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
	SFCLoader(sfc::SFCFile& sfc_)
		: sfc(sfc_) {}
	void load_everything();

	void load_objects_and_sceneries();

	ObjectHandle load_object(sfc::ObjectV1* p);
	std::shared_ptr<sfc::ObjectV1> dump_object(Object*) {
		throw Exception("Can't call dump_object on a SFCLoader");
	}

	constexpr bool is_loading() const { return true; }
	constexpr bool is_storing() const { return false; }

	sfc::SFCFile& sfc;
	std::map<const sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;
};

struct SFCSerializer {
	std::shared_ptr<sfc::ObjectV1> dump_object(Object*);
	ObjectHandle load_object(sfc::ObjectV1*) {
		throw Exception("Can't call load_object on a SFCSerializer");
	}

	sfc::SFCFile sfc;
	std::map<Object*, std::shared_ptr<sfc::ObjectV1>> sfc_object_mapping;

	constexpr bool is_loading() const { return false; }
	constexpr bool is_storing() const { return true; }
};

template <typename Ctx>
void serialize_viewport(Ctx&& ctx) {
	if (ctx.is_storing()) {
		ctx.sfc.scrollx = g_engine_context.viewport->get_scrollx();
		ctx.sfc.scrolly = g_engine_context.viewport->get_scrolly();
	} else {
		fmt::print("INFO [SFCLoader] Viewport {}, {}\n", ctx.sfc.scrollx, ctx.sfc.scrolly);
		g_engine_context.viewport->set_scroll_position(ctx.sfc.scrollx, ctx.sfc.scrolly);
	}
}

template <typename Ctx>
void serialize_map(Ctx&& ctx) {
	if (ctx.is_storing()) {
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

		ctx.sfc.map = map;
	} else {
		if (ctx.sfc.map->unused_is_wrappable != 0) {
			fmt::print("WARN [SFCLoader] unused_is_wrappable = {}, expected 0\n", ctx.sfc.map->unused_is_wrappable);
		}
		fmt::print("WARN [SFCLoader] Unsupported: time_of_day = {}\n", ctx.sfc.map->time_of_day);

		fmt::print("INFO [SFCLoader] Loading background = {}.spr\n", ctx.sfc.map->background->filename);
		auto background = g_engine_context.images->get_image(
			ctx.sfc.map->background->filename,
			ctx.sfc.map->background->absolute_base,
			numeric_cast<int32_t>(ctx.sfc.map->background->images.size()),
			static_cast<ImageManager::ImageType>(ImageManager::IMAGE_SPR | ImageManager::IMAGE_IS_BACKGROUND));
		if (ctx.sfc.map->background->absolute_base != 0) {
			fmt::print("WARN [SFCLoader] map background absolute base = {}, expected 0", ctx.sfc.map->background->absolute_base);
		}
		// TODO: do any C1 metarooms have non-standard sizes?
		if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
			throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
		}
		g_engine_context.map->set_background(background);

		fmt::print("INFO [SFCLoader] Loading rooms...\n");
		for (auto& r : ctx.sfc.map->rooms) {
			Room room;
			room.left = r.left;
			room.top = r.top;
			room.right = r.right;
			room.bottom = r.bottom;
			room.type = r.type;
			g_engine_context.map->add_room(std::move(room));
		}

		fmt::print("INFO [SFCLoader] Loading groundlevel...\n");
		g_engine_context.map->set_groundlevel(ctx.sfc.map->groundlevel);

		fmt::print("WARN [SFCLoader] Unsupported: bacteria\n");
	}
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

static Renderable sfc_load_renderable(const sfc::EntityV1* part) {
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

static std::shared_ptr<sfc::EntityV1> sfc_dump_renderable(const Renderable& r, const std::shared_ptr<sfc::CGalleryV1>& gallery = {}) {
	auto entity = std::make_shared<sfc::EntityV1>();
	if (gallery) {
		// so we can share gallery between all parts on a compound object / creature
		entity->gallery = gallery;
	} else {
		entity->gallery = sfc_dump_gallery(r.get_gallery());
	}
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

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::ObjectV1* p, Object* obj) {
	if (ctx.is_storing()) {
		p->species = obj->species;
		p->genus = obj->genus;
		p->family = obj->family;
		p->movement_status = obj->movement_status;
		p->attr = obj->attr;
		p->limit_left = obj->limit.x;
		p->limit_top = obj->limit.y;
		p->limit_right = obj->limit.right();
		p->limit_bottom = obj->limit.bottom();
		p->carrier = ctx.dump_object(g_engine_context.objects->try_get(obj->carrier)).get();
		p->actv = obj->actv;
		if (p->gallery == nullptr) {
			throw Exception("Expected gallery to be non-null by this point, it should have been"
							" set by a serialization function for a more specialized object type");
		}
		p->tick_value = obj->tick_value;
		p->ticks_since_last_tick_event = obj->ticks_since_last_tick_event;
		p->objp = ctx.dump_object(g_engine_context.objects->try_get(obj->objp)).get();
		if (obj->current_sound.get_looping()) {
			p->current_sound = obj->current_sound.get_name();
		}
		p->obv0 = obj->obv0;
		p->obv1 = obj->obv1;
		p->obv2 = obj->obv2;
		// TODO: I think we can skip scripts here, since they'll be loaded from the global
		// scriptorium anyways?
		// p->scripts = obj->scripts;
	} else {
		obj->species = p->species;
		obj->genus = p->genus;
		obj->family = p->family;
		obj->movement_status = MovementStatus(p->movement_status);
		obj->attr = p->attr;
		obj->limit.x = p->limit_left;
		obj->limit.y = p->limit_top;
		obj->limit.width = p->limit_right - p->limit_left;
		obj->limit.height = p->limit_bottom - p->limit_top;
		obj->carrier = ctx.load_object(p->carrier);
		obj->actv = ActiveFlag(p->actv);
		// creaturesImage sprite;
		obj->tick_value = p->tick_value;
		obj->ticks_since_last_tick_event = p->ticks_since_last_tick_event;
		// Set the sound later, once we've loaded the object's position from its Entities
		// obj->current_sound = p->current_sound;
		obj->objp = ctx.load_object(p->objp);
		obj->obv0 = p->obv0;
		obj->obv1 = p->obv1;
		obj->obv2 = p->obv2;

		// do this _after_ loading the SimpleObject or CompoundObject data,
		// since only then do we know the object's position / bounding box
		// TODO: should probably somewhere else? like global initialization after everything's been loaded?
		if (!p->current_sound.empty()) {
			// these won't be audible immediately, since the SoundManager thinks
			// they're out of hearing range. once the game starts and the
			// listener viewport gets set these will start being audible.
			obj->current_sound = g_engine_context.sounds->play_controlled_sound(p->current_sound, obj->get_bbox(), true);
		}
	}
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::SceneryV1* scen, Scenery* obj) {
	if (ctx.is_storing()) {
		scen->part = sfc_dump_renderable(obj->part);
		static_cast<sfc::ObjectV1*>(scen)->gallery = scen->part->gallery;
	} else {
		obj->part = sfc_load_renderable(scen->part.get());
	}
	serialize_object(ctx, static_cast<sfc::ObjectV1*>(scen), static_cast<Object*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::SimpleObjectV1* simp, SimpleObject* obj) {
	if (ctx.is_storing()) {
		simp->part = sfc_dump_renderable(obj->part);
		static_cast<sfc::ObjectV1*>(simp)->gallery = simp->part->gallery;
		simp->z_order = obj->z_order;
		simp->click_bhvr = obj->click_bhvr;
		simp->touch_bhvr = obj->touch_bhvr;
	} else {
		obj->part = sfc_load_renderable(simp->part.get());
		obj->z_order = simp->z_order;
		obj->click_bhvr = simp->click_bhvr;
		obj->touch_bhvr = simp->touch_bhvr;
	}
	serialize_object(ctx, static_cast<sfc::ObjectV1*>(simp), static_cast<Object*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::BubbleV1* bub, Bubble* obj) {
	if (ctx.is_storing()) {
	} else {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type: Bubble\n", obj->family, obj->genus, obj->species);
	}
	serialize_object(ctx, static_cast<sfc::SimpleObjectV1*>(bub), static_cast<SimpleObject*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::CallButtonV1* cbtn, CallButton* obj) {
	if (ctx.is_storing()) {
		// CallButtonV1
		cbtn->lift = dynamic_cast<sfc::LiftV1*>(
			ctx.dump_object(g_engine_context.objects->try_get(obj->lift)).get());
		cbtn->floor = obj->floor;
	} else {
		obj->lift = ctx.load_object(cbtn->lift);
		obj->floor = cbtn->floor;
	}
	serialize_object(ctx, static_cast<sfc::SimpleObjectV1*>(cbtn), static_cast<SimpleObject*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::PointerToolV1* pntr, PointerTool* obj) {
	if (ctx.is_storing()) {
		pntr->relx = obj->relx;
		pntr->rely = obj->rely;
		pntr->bubble = dynamic_cast<sfc::BubbleV1*>(
			ctx.dump_object(g_engine_context.objects->try_get(obj->bubble)).get());
		pntr->text = obj->text;
	} else {
		obj->relx = pntr->relx;
		obj->rely = pntr->rely;
		obj->bubble = ctx.load_object(pntr->bubble);
		obj->text = pntr->text;
		g_engine_context.pointer->m_pointer_tool = obj->uid;
	}
	serialize_object(ctx, static_cast<sfc::SimpleObjectV1*>(pntr), static_cast<SimpleObject*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::CompoundObjectV1* comp, CompoundObject* obj) {
	if (ctx.is_storing()) {
		for (auto& part : obj->parts) {
			sfc::CompoundPartV1 sfcpart;
			sfcpart.entity = sfc_dump_renderable(part.renderable, comp->gallery);
			if (!comp->gallery) {
				comp->gallery = sfcpart.entity->gallery;
			}
			sfcpart.x = part.x;
			sfcpart.y = part.y;
			comp->parts.push_back(sfcpart);
		}
		for (size_t i = 0; i < comp->hotspots.size(); ++i) {
			comp->hotspots[i].left = obj->hotspots[i].x;
			comp->hotspots[i].top = obj->hotspots[i].y;
			comp->hotspots[i].right = obj->hotspots[i].right();
			comp->hotspots[i].bottom = obj->hotspots[i].bottom();
		}
		comp->functions_to_hotspots = obj->functions_to_hotspots;
	} else {
		for (auto& cp : comp->parts) {
			obj->parts.emplace_back();
			obj->parts.back().renderable = sfc_load_renderable(cp.entity.get());
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
	serialize_object(ctx, static_cast<sfc::ObjectV1*>(comp), static_cast<Object*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::VehicleV1* veh, Vehicle* obj) {
	// recurse before the Vehicle logic (unlike the other functions) because
	// we need to adjust the parts' position based on the vehicle's fixed point 24.8 position.
	serialize_object(ctx, static_cast<sfc::CompoundObjectV1*>(veh), static_cast<CompoundObject*>(obj));

	if (ctx.is_storing()) {
		// TODO: round or truncate velocity? does it matter?
		veh->x_times_256 = numeric_cast<int32_t>(obj->parts[0].renderable.get_x() * 256);
		veh->y_times_256 = numeric_cast<int32_t>(obj->parts[0].renderable.get_y() * 256);
		veh->xvel_times_256 = numeric_cast<int32_t>(obj->xvel * 256);
		veh->yvel_times_256 = numeric_cast<int32_t>(obj->yvel * 256);
		veh->cabin_left = obj->cabin_left;
		veh->cabin_top = obj->cabin_top;
		veh->cabin_right = obj->cabin_right;
		veh->cabin_bottom = obj->cabin_bottom;
		veh->bump = obj->bump;
	} else {
		// need the compound parts to already be defined
		obj->xvel = veh->xvel_times_256 / 256.f;
		obj->yvel = veh->yvel_times_256 / 256.f;
		if (obj->parts.empty()) {
			throw Exception("Expected compound parts to be non-null by this point, they should have"
							" been loaded by the CompoundObject serialization logic");
		}
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
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::LiftV1* lift, Lift* obj) {
	if (ctx.is_storing()) {
		lift->num_floors = numeric_cast<int32_t>(obj->floors.size());
		lift->next_or_current_floor = obj->next_or_current_floor;

		// Would like to do this based on index w/in the serialized array, but
		// we're not guaranteed to have the CallButtons serialized by this point...
		// Since CallButtons have a pointer to their parent Lift, we might start
		// serializing a CallButton and immediately switch to serializing the Lift,
		// without fleshing out the CallButton struct until the Lift is finished.
		lift->current_call_button = index_if(obj->activated_call_buttons, [&](auto& handle) {
			auto* cb = g_engine_context.objects->try_get(handle);
			return cb && cb->as_call_button()->floor == lift->next_or_current_floor;
		});

		lift->delay_ticks_divided_by_32 = 0; // TODO
		for (size_t i = 0; i < obj->floors.size(); ++i) {
			lift->floors[i] = obj->floors[i];
		}
		for (auto it : enumerate(obj->activated_call_buttons)) {
			lift->activated_call_buttons[it.first] = dynamic_cast<sfc::CallButtonV1*>(
				ctx.dump_object(g_engine_context.objects->try_get(it.second)).get());
		}
	} else {
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
			if (auto&& handle = ctx.load_object(cb)) {
				obj->activated_call_buttons.insert(handle);
			}
		}
	}
	serialize_object(ctx, static_cast<sfc::VehicleV1*>(lift), static_cast<Vehicle*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::BlackboardV1* bbd, Blackboard* obj) {
	if (ctx.is_storing()) {
		bbd->background_color = obj->background_color;
		bbd->chalk_color = obj->chalk_color;
		bbd->alias_color = obj->alias_color;
		bbd->text_x_position = obj->text_x_position;
		bbd->text_y_position = obj->text_y_position;
		for (size_t i = 0; i < bbd->words.size(); ++i) {
			bbd->words[i].value = obj->words[i].value;
			bbd->words[i].text = obj->words[i].text;
		}
	} else {
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
	serialize_object(ctx, static_cast<sfc::CompoundObjectV1*>(bbd), static_cast<CompoundObject*>(obj));
}

template <typename Ctx>
static void serialize_object(Ctx&& ctx, sfc::CreatureV1* crea, Creature* obj) {
	if (ctx.is_storing()) {
	} else {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Creature\n", obj->family, obj->genus, obj->species);
	}
	serialize_object(ctx, static_cast<sfc::ObjectV1*>(crea), static_cast<Object*>(obj));
}

template <typename T, typename U>
static bool create_engine_object(SFCLoader* ctx, sfc::ObjectV1* p) {
	if (typeid(*p) != typeid(T)) {
		return false;
	}
	ObjectHandle handle = g_engine_context.objects->add<U>();
	ctx->sfc_object_mapping[p] = handle;
	auto* obj = g_engine_context.objects->try_get(handle);
	serialize_object(*ctx, static_cast<T*>(p), static_cast<U*>(obj));
	return true;
}

ObjectHandle SFCLoader::load_object(sfc::ObjectV1* p) {
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
	if (create_engine_object<sfc::SceneryV1, Scenery>(this, p)) {
	} else if (create_engine_object<sfc::SimpleObjectV1, SimpleObject>(this, p)) {
	} else if (create_engine_object<sfc::BubbleV1, Bubble>(this, p)) {
	} else if (create_engine_object<sfc::CallButtonV1, CallButton>(this, p)) {
	} else if (create_engine_object<sfc::PointerToolV1, PointerTool>(this, p)) {
	} else if (create_engine_object<sfc::CompoundObjectV1, CompoundObject>(this, p)) {
	} else if (create_engine_object<sfc::VehicleV1, Vehicle>(this, p)) {
	} else if (create_engine_object<sfc::LiftV1, Lift>(this, p)) {
	} else if (create_engine_object<sfc::BlackboardV1, Blackboard>(this, p)) {
	} else if (create_engine_object<sfc::CreatureV1, Creature>(this, p)) {
	} else {
		throw Exception("Unknown object type");
	}

	return sfc_object_mapping.find(p)->second;
}

void SFCLoader::load_objects_and_sceneries() {
	// this part's a little tricky, since SFC objects can encode pointers to other
	// objects. we keep a map of all sfc::ObjectV1* pointers we've seen so far, and
	// convert new pointers / load actual Objects on the fly. (note that references
	// can be cyclical, so the map must also keep a list of all pointers _in the process_
	// of being loaded.)
	for (auto& p : sfc.objects) {
		load_object(p.get());
	}
	fmt::print("INFO [SFCLoader] Loaded {} objects\n", sfc.objects.size());
	for (auto& p : sfc.sceneries) {
		load_object(p.get());
	}
	fmt::print("INFO [SFCLoader] Loaded {} sceneries\n", sfc.sceneries.size());
}

template <typename Ctx>
void serialize_scripts(Ctx&& ctx) {
	if (ctx.is_storing()) {
		for (auto& p : *g_engine_context.scriptorium) {
			sfc::ScriptV1 script;
			script.family = p.first.family;
			script.genus = p.first.genus;
			script.species = p.first.species;
			script.eventno = p.first.eventno;
			script.text = p.second;
			ctx.sfc.scripts.push_back(script);
		}
	} else {
		for (auto& s : ctx.sfc.scripts) {
			g_engine_context.scriptorium->add(s.family, s.genus, s.species, s.eventno, s.text);
		}
	}
}

template <typename Ctx>
void serialize_macros(Ctx&& ctx) {
	if (ctx.is_storing()) {
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
			mac->ownr = ctx.dump_object(g_engine_context.objects->try_get(m.ownr)).get();
			mac->from = ctx.dump_object(g_engine_context.objects->try_get(m.from)).get();
			mac->exec = ctx.dump_object(g_engine_context.objects->try_get(m.exec)).get();
			mac->targ = ctx.dump_object(g_engine_context.objects->try_get(m.targ)).get();
			mac->_it_ = ctx.dump_object(g_engine_context.objects->try_get(m._it_)).get();
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

			ctx.sfc.macros.push_back(mac);
		}
	} else {
		for (const auto& m : ctx.sfc.macros) {
			Macro macro;
			macro.selfdestruct = m->selfdestruct;
			macro.inst = m->inst;
			macro.script = m->script;
			macro.ip = m->ip;
			for (size_t i = 0; i < m->sp; ++i) {
				macro.stack.push_back(m->stack[i]);
			}
			macro.vars = m->vars;
			macro.ownr = ctx.load_object(m->ownr);
			macro.from = ctx.load_object(m->from);
			macro.exec = ctx.load_object(m->exec);
			macro.targ = ctx.load_object(m->targ);
			macro._it_ = ctx.load_object(m->_it_);
			macro.part = m->part;
			macro.subroutine_label = m->subroutine_label;
			macro.subroutine_address = m->subroutine_address;
			macro.wait = m->wait;

			g_engine_context.macros->add(macro);
		}
	}
}

void sfc_load_everything(const sfc::SFCFile& sfc) {
	SFCLoader loader(const_cast<sfc::SFCFile&>(sfc));
	loader.load_everything();
}

template <typename T, typename U>
bool dump_engine_object(SFCSerializer* ctx, Object* p) {
	if (typeid(*p) != typeid(U)) {
		return false;
	}
	auto obj = std::make_shared<T>();
	ctx->sfc_object_mapping[p] = obj;
	serialize_object(*ctx, obj.get(), static_cast<U*>(p));

	if (p->as_scenery()) {
		ctx->sfc.sceneries.push_back(std::dynamic_pointer_cast<sfc::SceneryV1>(obj));
	} else {
		ctx->sfc.objects.push_back(obj);
	}
	return true;
}

std::shared_ptr<sfc::ObjectV1> SFCSerializer::dump_object(Object* p) {
	if (!p) {
		return nullptr;
	}

	auto it = sfc_object_mapping.find(p);
	if (it != sfc_object_mapping.end()) {
		// already processed/processing this one
		return it->second;
	}

	if (dump_engine_object<sfc::SceneryV1, Scenery>(this, p)) {
	} else if (dump_engine_object<sfc::SimpleObjectV1, SimpleObject>(this, p)) {
	} else if (typeid(*p) == typeid(Bubble)) {
		fmt::print("WARN [SFCWriter] Unsupported type: Bubble\n");
		return nullptr;
	} else if (dump_engine_object<sfc::CallButtonV1, CallButton>(this, p)) {
	} else if (dump_engine_object<sfc::PointerToolV1, PointerTool>(this, p)) {
	} else if (dump_engine_object<sfc::CompoundObjectV1, CompoundObject>(this, p)) {
	} else if (dump_engine_object<sfc::VehicleV1, Vehicle>(this, p)) {
	} else if (dump_engine_object<sfc::LiftV1, Lift>(this, p)) {
	} else if (dump_engine_object<sfc::BlackboardV1, Blackboard>(this, p)) {
	} else if (typeid(*p) == typeid(Creature)) {
		fmt::print("WARN [SFCWriter] Unsupported type: Creature\n");
		return nullptr;
	}

	return sfc_object_mapping.find(p)->second;
};


static void sfc_dump_objects_and_sceneries(SFCSerializer& ctx) {
	// so, similarly to _loading_ from the SFC, this is tricky since SFC objects
	// can encode points to other objects. we keep a map of all ObjectHandles we've
	// seen so far, and convert ObjectHandles / copy them to the SFCFile on the fly.
	for (auto* p : *g_engine_context.objects) {
		ctx.dump_object(p);
	}
}

sfc::SFCFile sfc_dump_everything() {
	SFCSerializer ctx;

	fmt::print("INFO [SFCWriter] Writing map...\n");
	serialize_map(ctx);

	fmt::print("INFO [SFCWriter] Writing objects and sceneries...\n");
	sfc_dump_objects_and_sceneries(ctx);

	fmt::print("INFO [SFCWriter] Writing macros...\n");
	serialize_macros(ctx);

	fmt::print("INFO [SFCWriter] Writing scriptorium...\n");
	serialize_scripts(ctx);

	fmt::print("INFO [SFCWriter] Writing viewport...\n");
	serialize_viewport(ctx);

	return ctx.sfc;
}

void SFCLoader::load_everything() {
	fmt::print("INFO [SFCLoader] Loading map...\n");
	serialize_map(*this);

	fmt::print("INFO [SFCLoader] Loading objects and sceneries...\n");
	load_objects_and_sceneries();

	fmt::print("INFO [SFCLoader] Loading scriptorium...\n");
	serialize_scripts(*this);

	fmt::print("INFO [SFCLoader] Loading viewport...\n");
	serialize_viewport(*this);

	fmt::print("WARN [SFCLoader] Unsupported: current norn\n");
	fmt::print("WARN [SFCLoader] Unsupported: favorite places\n");
	fmt::print("WARN [SFCLoader] Unsupported: speech history\n");

	fmt::print("INFO [SFCLoader] Loading macros...\n");
	serialize_macros(*this);

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