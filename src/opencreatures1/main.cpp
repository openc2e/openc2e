#include "C1MusicManager.h"
#include "C1SoundManager.h"
#include "EventManager.h"
#include "ImageManager.h"
#include "MacroCommands.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "ObjectManager.h"
#include "PathManager.h"
#include "RenderableManager.h"
#include "SDLBackend.h"
#include "SFCLoader.h"
#include "Scriptorium.h"
#include "ViewportManager.h"
#include "common/Repr.h"
#include "fileformats/NewSFCFile.h"
#include "openc2e-audiobackend/SDLMixerBackend.h"
#include "openc2e-core/keycodes.h"

#include <SDL.h>
#include <chrono>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

class PointerManager {
  public:
	ObjectHandle m_pointer_tool;

	int m_screenx;
	int m_screeny;

	PointerManager(std::shared_ptr<ViewportManager> viewport, std::shared_ptr<ObjectManager> objects, std::shared_ptr<RenderableManager> renderables)
		: m_viewport(viewport), m_objects(objects), m_renderables(renderables) {}

	void update() {
		PointerTool* pointer = m_objects->try_get<PointerTool>(m_pointer_tool);
		Renderable* r = m_renderables->try_get(pointer->part);
		r->x = m_screenx + m_viewport->scrollx - pointer->relx;
		r->y = m_screeny + m_viewport->scrolly - pointer->rely;
	}

	void handle_event(const BackendEvent& event) {
		if (event.type == eventmousemove) {
			m_screenx = event.x;
			m_screeny = event.y;
		}
	}

	std::shared_ptr<ViewportManager> m_viewport;
	std::shared_ptr<ObjectManager> m_objects;
	std::shared_ptr<RenderableManager> m_renderables;
};

class TickManager {
  public:
	TickManager(std::shared_ptr<ObjectManager> objects, std::shared_ptr<EventManager> events)
		: m_objects(objects), m_events(events) {}

	void tick() {
		// check object tick
		for (auto& o : *m_objects.get()) {
			if (o->tick_value > 0) {
				o->ticks_since_last_tick_event += 1;
				if (o->ticks_since_last_tick_event >= o->tick_value) {
					o->ticks_since_last_tick_event = 0;

					m_events->queue_script(o, o, SCRIPT_TIMER);
					fmt::print("Fired timer script for {}, {}, {}\n", o->family, o->genus, o->species);
				}
			}
		}
	}

	std::shared_ptr<ObjectManager> m_objects;
	std::shared_ptr<EventManager> m_events;
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
	g_audio_backend->init(); // TODO: initialized early so SFC sounds can start.. is this right?
	auto g_path_manager = std::make_shared<PathManager>(datapath);
	auto g_image_manager = std::make_shared<ImageManager>(g_path_manager);
	auto g_music_manager = std::make_shared<C1MusicManager>(g_path_manager, g_audio_backend);
	auto g_viewport_manager = std::make_shared<ViewportManager>(g_backend);
	auto g_renderable_manager = std::make_shared<RenderableManager>();
	auto g_object_manager = std::make_shared<ObjectManager>(g_renderable_manager);
	auto g_pointer_manager = std::make_shared<PointerManager>(g_viewport_manager, g_object_manager, g_renderable_manager);
	auto g_map = std::make_shared<MapManager>();
	auto g_scriptorium = std::make_shared<Scriptorium>();
	auto g_macros = std::make_shared<MacroManager>();
	auto g_event_manager = std::make_shared<EventManager>(g_object_manager, g_macros, g_scriptorium);
	auto g_tick_manager = std::make_shared<TickManager>(g_object_manager, g_event_manager);
	auto g_sound_manager = std::make_shared<C1SoundManager>(g_audio_backend, g_path_manager, g_viewport_manager);

	g_macros->ctx.objects = g_object_manager.get();
	g_macros->ctx.renderables = g_renderable_manager.get();
	g_macros->ctx.events = g_event_manager.get();
	g_macros->ctx.sounds = g_sound_manager.get();

	install_default_commands(g_macros->ctx);

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
	std::chrono::time_point<std::chrono::steady_clock> time_of_last_tick{};

	SFCLoader loader(sfc);
	loader.load_viewport(g_viewport_manager);
	loader.load_map(g_map);
	loader.load_objects(g_object_manager, g_renderable_manager, g_image_manager, g_sound_manager);
	loader.load_scripts(g_scriptorium);
	loader.load_macros(g_macros);

	// find our pointer
	for (auto obj : g_object_manager->find_all<PointerTool>()) {
		g_pointer_manager->m_pointer_tool = obj;
	}

	// load background
	auto background_name = sfc.map->background->filename;
	fmt::print("* Background sprite: {}\n", repr(background_name));
	auto background = g_image_manager->get_image(background_name, ImageManager::IMAGE_SPR);
	// TODO: do any C1 metarooms have non-standard sizes?
	if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
	}

	// fire init scripts
	for (auto& o : *g_object_manager.get()) {
		g_event_manager->queue_script(o, o, SCRIPT_INITIALIZE);
	}

	// run loop
	g_backend->init();
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
			g_object_manager->tick();
			g_tick_manager->tick();
			g_macros->tick();
			g_renderable_manager->tick(); // after CAOS runs, otherwise the OVER command is too fast
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
		for (auto& r : g_renderable_manager->iter_zorder()) {
			int x = r.x - g_viewport_manager->scrollx;
			// what to do if it's near the wraparound? just draw three times?
			renderer->renderCreaturesImage(r.sprite, r.frame(), x, r.y - g_viewport_manager->scrolly);
			renderer->renderCreaturesImage(r.sprite, r.frame(), x - CREATURES1_WORLD_WIDTH, r.y - g_viewport_manager->scrolly);
			renderer->renderCreaturesImage(r.sprite, r.frame(), x + CREATURES1_WORLD_WIDTH, r.y - g_viewport_manager->scrolly);
		}
		// // draw rooms
		for (auto& room : g_map->rooms) {
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