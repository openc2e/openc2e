#include "SFCSerialization.h"

#include "C1SoundManager.h"
#include "EngineContext.h"
#include "ImageManager.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "PointerManager.h"
#include "Renderable.h"
#include "Scriptorium.h"
#include "ViewportManager.h"
#include "common/Ranges.h"
#include "fileformats/sfc/Blackboard.h"
#include "fileformats/sfc/Bubble.h"
#include "fileformats/sfc/CallButton.h"
#include "fileformats/sfc/Creature.h"
#include "fileformats/sfc/Lift.h"
#include "fileformats/sfc/Macro.h"
#include "fileformats/sfc/MapData.h"
#include "fileformats/sfc/PointerTool.h"
#include "fileformats/sfc/SFCFile.h"
#include "fileformats/sfc/Scenery.h"
#include "fileformats/sfc/SimpleObject.h"
#include "objects/Blackboard.h"
#include "objects/Bubble.h"
#include "objects/CallButton.h"
#include "objects/CompoundObject.h"
#include "objects/Creature.h"
#include "objects/Lift.h"
#include "objects/ObjectManager.h"
#include "objects/PointerTool.h"
#include "objects/Scenery.h"
#include "objects/SimpleObject.h"
#include "objects/Vehicle.h"

#include <fmt/ranges.h>

struct SFCLoader : SFCContext {
	SFCLoader(sfc::SFCFile& sfc_)
		: sfc(sfc_) {}
	void load_everything();

	void load_objects_and_sceneries();

	ObjectHandle load_object(sfc::ObjectV1* p) override;
	std::shared_ptr<sfc::ObjectV1> dump_object(Object*) override {
		throw Exception("Can't call dump_object on a SFCLoader");
	}

	bool is_loading() const override { return true; }
	bool is_storing() const override { return false; }

	sfc::SFCFile& sfc;
	std::map<const sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;
};

struct SFCSerializer : SFCContext {
	std::shared_ptr<sfc::ObjectV1> dump_object(Object*) override;
	ObjectHandle load_object(sfc::ObjectV1*) override {
		throw Exception("Can't call load_object on a SFCSerializer");
	}

	sfc::SFCFile sfc;
	std::map<Object*, std::shared_ptr<sfc::ObjectV1>> sfc_object_mapping;

	bool is_loading() const override { return false; }
	bool is_storing() const override { return true; }
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
			room.rect.left = r.left;
			room.rect.top = r.top;
			room.rect.right = r.right;
			room.rect.bottom = r.bottom;
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
			room.left = r.rect.left;
			room.top = r.rect.top;
			room.right = r.rect.right;
			room.bottom = r.rect.bottom;
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

Renderable sfc_load_renderable(const sfc::EntityV1* part) {
	if (part->x >= CREATURES1_WORLD_WIDTH) {
		throw Exception(fmt::format("Expected x to be between [0, {}), but got {}", CREATURES1_WORLD_WIDTH, part->x));
	}
	if (part->y >= CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected y to be between [0, {}), but got {}", CREATURES1_WORLD_HEIGHT, part->y));
	}

	auto gallery = g_engine_context.images->get_image(
		part->gallery->filename,
		part->gallery->absolute_base,
		numeric_cast<int32_t>(part->gallery->images.size()),
		ImageManager::IMAGE_SPR);


	Renderable r(gallery, part->sprite_base, part->x, part->y, part->z_order);
	r.set_pose(part->sprite_pose_plus_base - part->sprite_base);
	if (part->has_animation) {
		r.set_animation(part->animation_frame, part->animation_string);
	}
	return r;
}

std::shared_ptr<sfc::EntityV1> sfc_dump_renderable(const Renderable& r, const std::shared_ptr<sfc::CGalleryV1>& gallery) {
	auto entity = std::make_shared<sfc::EntityV1>();
	if (gallery) {
		// so we can share gallery between all parts on a compound object / creature
		entity->gallery = gallery;
	} else {
		entity->gallery = sfc_dump_gallery(r.gallery());
	}
	entity->sprite_pose_plus_base = numeric_cast<uint8_t>(r.pose() + r.base());
	entity->sprite_base = numeric_cast<uint8_t>(r.base());
	entity->z_order = r.z_order();
	entity->x = numeric_cast<int32_t>(r.x());
	entity->y = numeric_cast<int32_t>(r.y());
	entity->has_animation = r.has_animation();
	if (entity->has_animation) {
		entity->animation_frame = r.animation_frame(); // only if has_animation is true
		entity->animation_string = r.animation_string();
	}

	return entity;
}

template <typename T, typename U>
static bool create_engine_object(SFCLoader* ctx, sfc::ObjectV1* p) {
	if (typeid(*p) != typeid(T)) {
		return false;
	}
	ObjectHandle handle = g_engine_context.objects->add<U>();
	ctx->sfc_object_mapping[p] = handle;
	auto* obj = g_engine_context.objects->try_get(handle);
	static_cast<U*>(obj)->serialize(*ctx, static_cast<T*>(p));
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
		// } else if (create_engine_object<sfc::CreatureV1, Creature>(this, p)) {
	} else if (typeid(*p) == typeid(sfc::CreatureV1)) {
		fmt::print("WARN [SFCLoader] Unsupported type: Creature\n");
		return {};
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
	static_cast<U*>(p)->serialize(*ctx, obj.get());

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