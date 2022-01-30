#include "C1MusicManager.h"
#include "ImageManager.h"
#include "PathManager.h"
#include "SDLBackend.h"
#include "common/Ascii.h"
#include "common/Repr.h"
#include "common/case_insensitive_filesystem.h"
#include "common/optional.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/NewSfcFile.h"
#include "fileformats/paletteFile.h"
#include "openc2e-audiobackend/SDLMixerBackend.h"
#include "openc2e-core/creaturesImage.h"
#include "openc2e-core/keycodes.h"

#include <SDL.h>
#include <chrono>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

static constexpr int CREATURES1_WORLD_WIDTH = 8352;
static constexpr int CREATURES1_WORLD_HEIGHT = 1200;

class ViewportManager {
  public:
	ViewportManager(std::shared_ptr<Backend> backend)
		: m_backend(backend) {}
	void handle_event(const BackendEvent& event) {
		if (!(event.type == eventrawkeyup || event.type == eventrawkeydown)) {
			return;
		}
		if (event.key == OPENC2E_KEY_LEFT) {
			scroll_left = (event.type == eventrawkeydown);
		} else if (event.key == OPENC2E_KEY_RIGHT) {
			scroll_right = (event.type == eventrawkeydown);
		} else if (event.key == OPENC2E_KEY_UP) {
			scroll_up = (event.type == eventrawkeydown);
		} else if (event.key == OPENC2E_KEY_DOWN) {
			scroll_down = (event.type == eventrawkeydown);
		}
	}
	void tick() {
		// scroll left-right
		if (scroll_left) {
			scroll_velx -= SCROLL_ACCEL;
		}
		if (scroll_right) {
			scroll_velx += SCROLL_ACCEL;
		}
		if (!scroll_left && !scroll_right) {
			scroll_velx *= SCROLL_DECEL;
			if (-0.1 < scroll_velx && scroll_velx < 0.1) {
				scroll_velx = 0;
			}
		}
		// scroll up-down
		if (scroll_up) {
			scroll_vely -= SCROLL_ACCEL;
		}
		if (scroll_down) {
			scroll_vely += SCROLL_ACCEL;
		}
		if (!scroll_up && !scroll_down) {
			scroll_vely *= SCROLL_DECEL;
			if (-0.1 < scroll_vely && scroll_vely < 0.1) {
				scroll_vely = 0;
			}
		}
		// enforce scroll speed limits
		if (scroll_velx >= SCROLL_MAX) {
			scroll_velx = SCROLL_MAX;
		} else if (scroll_velx <= -SCROLL_MAX) {
			scroll_velx = -SCROLL_MAX;
		}
		if (scroll_vely >= SCROLL_MAX) {
			scroll_vely = SCROLL_MAX;
		} else if (scroll_vely <= -SCROLL_MAX) {
			scroll_vely = -SCROLL_MAX;
		}

		// do the actual movement
		if (scroll_velx || scroll_vely) {
			scrollx += scroll_velx;
			scrolly += scroll_vely;
		}

		// fix scroll
		int window_height = m_backend->getMainRenderTarget()->getHeight();
		// can't go past top or bottom
		if (scrolly < 0) {
			scrolly = 0;
			scroll_vely = 0;
		} else if (CREATURES1_WORLD_HEIGHT - scrolly < window_height) {
			scrolly = CREATURES1_WORLD_HEIGHT - window_height;
			scroll_vely = 0;
		}
		// wraparound left and right
		if (scrollx < 0) {
			scrollx = CREATURES1_WORLD_WIDTH + scrollx;
		} else if (scrollx >= CREATURES1_WORLD_WIDTH) {
			scrollx -= CREATURES1_WORLD_WIDTH;
		}
	}
	int scrollx = 0;
	int scrolly = 0;

  private:
	static constexpr float SCROLL_ACCEL = 8;
	static constexpr float SCROLL_DECEL = 0.5;
	static constexpr float SCROLL_MAX = 64;

	bool scroll_left = false;
	bool scroll_right = false;
	bool scroll_up = false;
	bool scroll_down = false;

	float scroll_velx = 0;
	float scroll_vely = 0;

	std::shared_ptr<Backend> m_backend;
};


class Entity {
  public:
	int x;
	int y;
	int z;
	int object_sprite_base;
	int part_sprite_base;
	int sprite_index;
	creaturesImage sprite;
	bool has_animation;
	unsigned int animation_frame; // only if has_animation is true
	std::string animation_string; // only if has_animation is true
};

class EntityHandle {
  private:
	friend class EntityManager;
	size_t id = static_cast<size_t>(-1);
};

class EntityManager {
  public:
	EntityHandle add_entity(const Entity& e) {
		m_entities.push_back(e);
		m_entities_zorder.clear();
		EntityHandle eh;
		eh.id = m_entities.size() - 1;
		return eh;
	}

	void set_entity_x(EntityHandle handle, int x) {
		// TODO: check world bounds
		m_entities[handle.id].x = x;
	}
	void set_entity_y(EntityHandle handle, int y) {
		// TODO: check world bounds
		m_entities[handle.id].y = y;
	}

	void tick() {
		for (auto& e : m_entities) {
			if (!e.has_animation) {
				continue;
			}
			if (e.animation_frame >= e.animation_string.size()) {
				// already done
				// TODO: are we on the correct frame already?
				// TODO: clear animation?
				e.has_animation = false;
				e.animation_string = {};
				e.animation_frame = 0;
				continue;
			}
			e.animation_frame += 1;
			if (e.animation_frame >= e.animation_string.size()) {
				// done!
				continue;
			}
			if (e.animation_string[e.animation_frame] == 'R') {
				e.animation_frame = 0;
			}

			// TODO: assert isdigit
			e.sprite_index = e.animation_string[e.animation_frame] - '0';
		}
	}

	const std::vector<Entity*>& entities_zorder() {
		if (m_entities_zorder.empty()) {
			fmt::print("* [EntityManager] Resorting entities by z-order\n");
			for (auto& e : m_entities) {
				m_entities_zorder.push_back(&e);
			}
			std::sort(m_entities_zorder.begin(), m_entities_zorder.end(), [](Entity* left, Entity* right) { return left->z < right->z; });
		}
		return m_entities_zorder;
	}

  private:
	std::vector<Entity> m_entities;
	std::vector<Entity*> m_entities_zorder;
};

static Entity entity_from_sfc(std::shared_ptr<ImageManager> image_manager, sfc::EntityV1& part) {
	if (part.x >= CREATURES1_WORLD_WIDTH) {
		throw Exception(fmt::format("Expected x to be between [0, {}), but got {}", CREATURES1_WORLD_WIDTH, part.x));
	}
	if (part.y >= CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected y to be between [0, {}), but got {}", CREATURES1_WORLD_HEIGHT, part.y));
	}

	Entity e;
	e.x = part.x;
	e.y = part.y;
	e.z = part.z_order;
	e.object_sprite_base = part.sprite->first_sprite;
	e.part_sprite_base = part.image_offset;
	e.sprite_index = part.current_sprite - part.image_offset;
	e.sprite = image_manager->get_image(part.sprite->filename, ImageManager::IMAGE_SPR);
	e.has_animation = part.has_animation;
	if (part.has_animation) {
		e.animation_frame = part.animation_frame;
		e.animation_string = part.animation_string;
	}
	return e;
}

class PointerManager {
  public:
	int32_t relx;
	int32_t rely;
	EntityHandle entity;
	// BubbleV1* bubble;
	// std::string text;

	int screenx;
	int screeny;

	PointerManager(std::shared_ptr<ViewportManager> viewport_manager, std::shared_ptr<EntityManager> entity_manager)
		: m_viewport_manager(viewport_manager), m_entity_manager(entity_manager) {}

	void update() {
		m_entity_manager->set_entity_x(entity, screenx + m_viewport_manager->scrollx - relx);
		m_entity_manager->set_entity_y(entity, screeny + m_viewport_manager->scrolly - rely);
	}

	void handle_event(const BackendEvent& event) {
		if (event.type == eventmousemove) {
			screenx = event.x;
			screeny = event.y;
		}
	}

	std::shared_ptr<ViewportManager> m_viewport_manager;
	std::shared_ptr<EntityManager> m_entity_manager;
};

enum MovementStatus {
	MOVEMENT_AUTONOMOUS = 0,
	MOVEMENT_MOUSEDRIVEN = 1,
	MOVEMENT_FLOATING = 2,
	MOVEMENT_INVEHICLE = 3,
	MOVEMENT_CARRIED = 4,
};

class ObjectHandle {
  public:
	uint32_t id() const {
		return static_cast<uint32_t>(m_type) | static_cast<uint32_t>(m_index << 4) | static_cast<uint32_t>(m_generation << 12);
	}

  private:
	friend class ObjectManager;
	uint8_t m_type : 4;
	uint16_t m_index : 12;
	uint16_t m_generation;
};
// static_assert(sizeof(ObjectHandle) == 4, "");

class Object {
  public:
	virtual ~Object() = default;
	uint8_t species;
	uint8_t genus;
	uint8_t family;
	MovementStatus movement_status;
	uint8_t attr;
	uint32_t limit_left;
	uint32_t limit_top;
	uint32_t limit_right;
	uint32_t limit_bottom;
	ObjectHandle carrier;
	uint8_t actv;
	// creaturesImage sprite;
	uint32_t tick_time;
	uint32_t tick_state;
	ObjectHandle objp;
	// std::string current_sound;
	uint32_t obv0;
	uint32_t obv1;
	uint32_t obv2;
};

static Object object_from_sfc(const sfc::ObjectV1& p, const std::map<sfc::ObjectV1*, ObjectHandle> sfc_object_mapping) {
	Object obj;
	obj.species = p.species;
	obj.genus = p.genus;
	obj.family = p.family;
	obj.movement_status = static_cast<MovementStatus>(p.movement_status);
	obj.attr = p.attr;
	obj.limit_left = p.limit_left;
	obj.limit_top = p.limit_top;
	obj.limit_right = p.limit_right;
	obj.limit_bottom = p.limit_bottom;
	obj.carrier = sfc_object_mapping.at(p.carrier);
	obj.actv = p.actv;
	// creaturesImage sprite;
	obj.tick_time = p.tick_time;
	obj.tick_state = p.tick_state;
	obj.objp = sfc_object_mapping.at(p.objp);
	// std::string current_sound;
	obj.obv0 = p.obv0;
	obj.obv1 = p.obv1;
	obj.obv2 = p.obv2;
	return obj;
}

class Scenery : public Object {
  public:
	EntityHandle part;
};

enum ObjectType {
	OBJECTTYPE_SCENERY = 1,
	OBJECTTYPE_OBJECT = 15, // TODO: remove
};

class ObjectManager {
	//     // What's going on here?
	//     // (1) First of all, we like to use handles to things. They let us store
	//     // data contiguously rather than having random pointers flying around all
	//     // over the place. You'll see this pattern in other places in this codebase.
	//     // (2) Secondly, and more importantly, we have multiple object types and
	//     // cannot store them all in a single array. Instead, we take a trick from
	//     // entity component systems' "archetypes." An archetype is essentially a bag
	//     // of components that appear together. An ECS will split entity storage across
	//     // multiple different arrays, where each array holds a single archetype.
	//     // In our case, we don't have components, but we do have a static object
	//     // hierarchy which looks somewhat similar to archetypes and is amenable to
	//     // similar techniques.
	//     //
	//     // The object hierarchy:
	//     //     Object (never used alone)
	//     //     |- Scenery
	//     //     |- Creature
	//     //     +- SimpleObject
	//     //     |  |- CallButton
	//     //     |  |- PointerTool
	//     //     +- CompoundObject
	//     //        |- Blackboard
	//     //        +- Vehicle
	//     //           |- Lift
	//
  public:
	ObjectHandle create(ObjectType type) {
		ObjectHandle handle;
		handle.m_type = type;
		if (type == OBJECTTYPE_OBJECT) {
			m_objects.emplace_back();
			handle.m_index = m_objects.size() - 1;
		} else if (type == OBJECTTYPE_SCENERY) {
			m_sceneries.emplace_back();
			handle.m_index = m_sceneries.size() - 1;
		}
		handle.m_generation = 1; // ???
		return handle;
	}

	Object& get_object(ObjectHandle handle) {
		if (handle.m_type == OBJECTTYPE_OBJECT) {
			return m_objects[handle.m_index];
		} else if (handle.m_type == OBJECTTYPE_SCENERY) {
			return m_sceneries[handle.m_index];
		} else {
			throw Exception(fmt::format("Bad ObjectHandle type {}", (int)handle.m_type));
		}
	}

	Scenery& get_scenery(ObjectHandle handle) {
		return dynamic_cast<Scenery&>(get_object(handle));
	}

  private:
	std::vector<Object> m_objects; // TODO: remove
	std::vector<Scenery> m_sceneries;

	//     ObjectHandle add_scenery(Scenery& s) {
	//
	//     }
	//
	//     std::vector<Scenery> m_sceneries;
	//     // std::vector<Creature> m_creatures;
	//     // std::vector<SimpleObject> m_simple_objects;
	//     // std::vector<CallButton> m_call_buttons;
	//     // std::vector<PointerTool> m_pointer_tools; // single pointer tool?
	//     // std::vector<CompoundObject> m_compound_objects;
	//     // std::vector<Blackboard> m_blackboards;
	//     // std::vector<Vehicle> m_vehicles;
	//     // std::vector<Lift> m_lifts;
};

class Room {
  public:
	int left;
	int top;
	int right;
	int bottom;
	int type;
};

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "Usage: {} path-to-creatures1-data\n", argv[0]);
		return 1;
	}

	std::string datapath = argv[1];
	if (!fs::exists(datapath)) {
		fmt::print(stderr, "* Error: Data path {} does not exist\n", repr(datapath));
		return 1;
	}

	fmt::print("* Creatures 1 Data: {}\n", repr(datapath));

	// set up global objects
	auto g_backend = std::make_shared<SDLBackend>();
	auto g_audio_backend = SDLMixerBackend::getInstance();
	auto g_path_manager = std::make_shared<PathManager>(datapath);
	auto g_image_manager = std::make_shared<ImageManager>(g_path_manager);
	auto g_music_manager = std::make_shared<C1MusicManager>(g_path_manager, g_audio_backend);
	auto g_viewport_manager = std::make_shared<ViewportManager>(g_backend);
	auto g_entity_manager = std::make_shared<EntityManager>();
	auto g_pointer_manager = std::make_shared<PointerManager>(g_viewport_manager, g_entity_manager);

	// load palette
	g_image_manager->load_default_palette();

	// load Eden.sfc
	auto eden_sfc_path = g_path_manager->find_path(PATH_TYPE_BASE, "Eden.sfc");
	if (eden_sfc_path.empty()) {
		fmt::print(stderr, "* Error: Couldn't find Eden.sfc\n");
		return 1;
	}
	fmt::print("* Found Eden.sfc: {}\n", repr(eden_sfc_path));
	auto sfc = sfc::read_sfc_v1_file(eden_sfc_path);

	// load world data
	g_viewport_manager->scrollx = sfc.scrollx;
	g_viewport_manager->scrolly = sfc.scrolly;
	std::chrono::time_point<std::chrono::steady_clock> time_of_last_tick{};

	// for (auto* scen : sfc.sceneries) {
	// 	g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *scen->part));
	// }
	for (auto* obj : sfc.objects) {
		if (auto* ptr = dynamic_cast<sfc::PointerToolV1*>(obj)) {
			auto entity = g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *ptr->part));
			g_pointer_manager->entity = entity;
			g_pointer_manager->relx = ptr->relx;
			g_pointer_manager->rely = ptr->rely;
		} else if (auto* simp = dynamic_cast<sfc::SimpleObjectV1*>(obj)) {
			g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *simp->part));
		} else if (auto* comp = dynamic_cast<sfc::CompoundObjectV1*>(obj)) {
			for (auto& part : comp->parts) {
				g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *part.entity));
			}
		}
	}

	std::vector<Room> rooms;
	for (auto& r : sfc.map->rooms) {
		Room room;
		room.left = r.left;
		room.top = r.top;
		room.right = r.right;
		room.bottom = r.bottom;
		room.type = r.type;
		rooms.push_back(room);
	}

	auto g_object_manager = std::make_shared<ObjectManager>();

	std::map<sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;
	sfc_object_mapping[nullptr] = {};
	for (auto* p : sfc.sceneries) {
		sfc_object_mapping[p] = g_object_manager->create(OBJECTTYPE_SCENERY);
	}
	for (auto* p : sfc.objects) {
		ObjectType type;
		// if (dynamic_cast<sfc::SceneryV1*>(p)) {
		// type = OBJECTTYPE_SCENERY;
		// } else {
		type = OBJECTTYPE_OBJECT;
		// }
		sfc_object_mapping[p] = g_object_manager->create(type);
	}
	for (auto* p : sfc.objects) {
		ObjectHandle handle = sfc_object_mapping[p];
		Object& obj = g_object_manager->get_object(handle);
		obj = object_from_sfc(*p, sfc_object_mapping);
	}
	for (auto* p : sfc.sceneries) {
		ObjectHandle handle = sfc_object_mapping[p];
		Object& obj = g_object_manager->get_object(handle);
		obj = object_from_sfc(*p, sfc_object_mapping);

		auto* s = dynamic_cast<sfc::SceneryV1*>(p);
		Scenery& scen = g_object_manager->get_scenery(handle);
		scen.part = g_entity_manager->add_entity(entity_from_sfc(g_image_manager, *s->part));
	}

	for (auto* m : sfc.macros) {
		ObjectHandle handle = sfc_object_mapping[m->ownr];
		Object& ownr = g_object_manager->get_object(handle);
		fmt::print("macro {} {} {} object({}),{} $ip$ {}\n", ownr.family, ownr.genus, ownr.species, handle.id(), m->script.substr(0, m->ip), m->script.substr(m->ip));
	}

	// for (auto & s : sfc.scripts) {
	//     fmt::print("scrp {} {} {} {},{}\n", s.family, s.genus, s.species, s.eventno, s.text);
	// }


	// load background
	auto background_name = sfc.map->background->filename;
	fmt::print("* Background sprite: {}\n", repr(background_name));
	auto background = g_image_manager->get_image(background_name, ImageManager::IMAGE_SPR);
	// TODO: do any C1 metarooms have non-standard sizes?
	if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
	}

	// run loop
	g_backend->init();
	g_audio_backend->init();
	uint32_t last_frame_end = SDL_GetTicks();
	while (true) {
		// handle ui events
		BackendEvent event;
		bool should_quit = false;
		while (g_backend->pollEvent(event)) {
			g_viewport_manager->handle_event(event);
			g_pointer_manager->handle_event(event);
			if (event.type == eventquit) {
				should_quit = true;
			}
		}
		if (should_quit) {
			break;
		}

		// update world
		// // some things can update only every "tick" - 1/10sec
		auto time_since_last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - time_of_last_tick)
										.count();
		if (time_since_last_tick >= 100) {
			time_of_last_tick = std::chrono::steady_clock::now();
			g_viewport_manager->tick();
			g_entity_manager->tick();
		}
		// // some things can update as often as possible
		g_music_manager->update();
		g_pointer_manager->update();

		// draw
		auto renderer = g_backend->getMainRenderTarget();
		// // draw world (twice, to handle wraparound)
		renderer->renderCreaturesImage(background, 0, -g_viewport_manager->scrollx, -g_viewport_manager->scrolly);
		renderer->renderCreaturesImage(background, 0, -g_viewport_manager->scrollx + CREATURES1_WORLD_WIDTH, -g_viewport_manager->scrolly);
		// // draw entities
		for (auto* part : g_entity_manager->entities_zorder()) {
			int x = part->x - g_viewport_manager->scrollx;
			int frame = part->object_sprite_base + part->part_sprite_base + part->sprite_index;
			// what to do if it's near the wraparound? just draw three times?
			renderer->renderCreaturesImage(part->sprite, frame, x, part->y - g_viewport_manager->scrolly);
			renderer->renderCreaturesImage(part->sprite, frame, x - CREATURES1_WORLD_WIDTH, part->y - g_viewport_manager->scrolly);
			renderer->renderCreaturesImage(part->sprite, frame, x + CREATURES1_WORLD_WIDTH, part->y - g_viewport_manager->scrolly);
		}
		// // draw rooms
		for (auto& room : rooms) {
			// what to do if it's near the wraparound? just draw three times?
			for (auto offset : {-CREATURES1_WORLD_WIDTH, 0, CREATURES1_WORLD_WIDTH}) {
				auto left = room.left - g_viewport_manager->scrollx + offset;
				auto top = room.top - g_viewport_manager->scrolly;
				auto right = room.right - g_viewport_manager->scrollx + offset;
				auto bottom = room.bottom - g_viewport_manager->scrolly;
				int color;
				if (room.type == 0) {
					color = 0xFFFF00CC;
				} else if (room.type == 1) {
					color = 0x00FFFFCC;
				} else {
					color = 0xFF00FFCC;
				}
				renderer->renderLine(left, top, right, top, color);
				renderer->renderLine(right, top, right, bottom, color);
				renderer->renderLine(left, top, left, bottom, color);
				renderer->renderLine(left, bottom, right, bottom, color);
			}
		}

		// present and wait
		SDL_RenderPresent(g_backend->renderer);

		static constexpr int OPENC2E_MAX_FPS = 60;
		static constexpr int OPENC2E_MS_PER_FRAME = 1000 / OPENC2E_MAX_FPS;

		Uint32 frame_end = SDL_GetTicks();
		if (frame_end - last_frame_end < OPENC2E_MS_PER_FRAME) {
			SDL_Delay(OPENC2E_MS_PER_FRAME - (frame_end - last_frame_end));
		}
		last_frame_end = frame_end;
	}

	return 0;
}