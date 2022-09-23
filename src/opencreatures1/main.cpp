#include "C1MusicManager.h"
#include "C1SoundManager.h"
#include "EngineContext.h"
#include "EventManager.h"
#include "ImageManager.h"
#include "MacroCommands.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "ObjectManager.h"
#include "PathManager.h"
#include "RenderableManager.h"
#include "SFCLoader.h"
#include "Scriptorium.h"
#include "TimerSystem.h"
#include "ViewportManager.h"
#include "common/Repr.h"
#include "common/backend/Keycodes.h"
#include "common/backtrace.h"
#include "fileformats/NewSFCFile.h"
#include "sdlbackend/SDLBackend.h"
#include "sdlbackend/SDLMixerBackend.h"

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


// SDL tries stealing main on some platforms, which we don't want.
#undef main

extern "C" int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "Usage: {} path-to-creatures1-data\n", argv[0]);
		return 1;
	}

	install_backtrace_printer();

	std::string datapath = argv[1];
	if (!fs::exists(datapath)) {
		fmt::print(stderr, "* Error: Data path {} does not exist\n", repr(datapath));
		return 1;
	}

	fmt::print("* Creatures 1 Data: {}\n", repr(datapath));

	// set up global objects
	auto g_engine_context = std::make_shared<EngineContext>();

	g_engine_context->backend = std::make_shared<SDLBackend>();
	g_engine_context->audio_backend = SDLMixerBackend::getInstance();
	g_engine_context->audio_backend->init(); // TODO: initialized early so SFC sounds can start.. is this right?

	g_engine_context->paths = std::make_shared<PathManager>(datapath);
	g_engine_context->images = std::make_shared<ImageManager>(g_engine_context->paths);
	g_engine_context->music = std::make_shared<C1MusicManager>(g_engine_context->paths, g_engine_context->audio_backend);
	g_engine_context->viewport = std::make_shared<ViewportManager>(g_engine_context->backend);
	g_engine_context->renderables = std::make_shared<RenderableManager>();
	g_engine_context->objects = std::make_shared<ObjectManager>(g_engine_context->renderables);
	g_engine_context->pointer = std::make_shared<PointerManager>(g_engine_context->viewport, g_engine_context->objects, g_engine_context->renderables);
	g_engine_context->map = std::make_shared<MapManager>();
	g_engine_context->scriptorium = std::make_shared<Scriptorium>();
	g_engine_context->macros = std::make_shared<MacroManager>();
	g_engine_context->events = std::make_shared<EventManager>(g_engine_context.get());
	g_engine_context->timers = std::make_shared<TimerSystem>();
	g_engine_context->sounds = std::make_shared<C1SoundManager>(g_engine_context->audio_backend, g_engine_context->paths, g_engine_context->viewport);

	g_engine_context->macros->ctx.objects = g_engine_context->objects.get();
	g_engine_context->macros->ctx.renderables = g_engine_context->renderables.get();
	g_engine_context->macros->ctx.events = g_engine_context->events.get();
	g_engine_context->macros->ctx.sounds = g_engine_context->sounds.get();

	install_default_commands(g_engine_context->macros->ctx);

	// load palette
	g_engine_context->images->load_default_palette();

	// load Eden.sfc
	auto eden_sfc_path = g_engine_context->paths->find_path(PATH_TYPE_BASE, "Eden.sfc");
	if (eden_sfc_path.empty()) {
		fmt::print(stderr, "* Error: Couldn't find Eden.sfc\n");
		return 1;
	}
	fmt::print("* Found Eden.sfc: {}\n", repr(eden_sfc_path));
	auto sfc = sfc::read_sfc_v1_file(eden_sfc_path);

	// load world data
	std::chrono::time_point<std::chrono::steady_clock> time_of_last_tick{};

	SFCLoader loader(sfc);
	loader.load_viewport(g_engine_context->viewport);
	loader.load_map(g_engine_context->map);
	loader.load_objects(g_engine_context->objects, g_engine_context->renderables, g_engine_context->images, g_engine_context->sounds);
	loader.load_scripts(g_engine_context->scriptorium);
	loader.load_macros(g_engine_context->macros);

	// find our pointer
	for (auto obj : g_engine_context->objects->find_all<PointerTool>()) {
		g_engine_context->pointer->m_pointer_tool = obj;
	}

	// load background
	auto background_name = sfc.map->background->filename;
	fmt::print("* Background sprite: {}\n", repr(background_name));
	auto background = g_engine_context->images->get_image(background_name, ImageManager::IMAGE_SPR);
	// TODO: do any C1 metarooms have non-standard sizes?
	if (background.width(0) != CREATURES1_WORLD_WIDTH || background.height(0) != CREATURES1_WORLD_HEIGHT) {
		throw Exception(fmt::format("Expected Creatures 1 background size to be 8352x1200 but got {}x{}", background.width(0), background.height(0)));
	}

	// fire init scripts
	for (auto& o : *g_engine_context->objects) {
		g_engine_context->events->queue_script(o, o, SCRIPT_INITIALIZE);
	}

	// run loop
	g_engine_context->backend->init("opencreatures1");
	while (true) {
		g_engine_context->backend->waitForNextDraw();

		// handle ui events
		BackendEvent event;
		bool should_quit = false;
		while (g_engine_context->backend->pollEvent(event)) {
			g_engine_context->viewport->handle_event(event);
			g_engine_context->pointer->handle_event(event);
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
			g_engine_context->viewport->tick();
			g_engine_context->objects->tick();
			g_engine_context->timers->tick(g_engine_context.get());
			g_engine_context->macros->tick();
			g_engine_context->renderables->tick(); // after CAOS runs, otherwise the OVER command is too fast
		}
		// // some things can update as often as possible
		g_engine_context->music->update();
		g_engine_context->pointer->update();

		// draw
		auto renderer = g_engine_context->backend->getMainRenderTarget();
		// // draw world (twice, to handle wraparound)
		renderer->renderCreaturesImage(background, 0, -g_engine_context->viewport->scrollx, -g_engine_context->viewport->scrolly);
		renderer->renderCreaturesImage(background, 0, -g_engine_context->viewport->scrollx + CREATURES1_WORLD_WIDTH, -g_engine_context->viewport->scrolly);
		// // draw entities
		for (auto& r : g_engine_context->renderables->iter_zorder()) {
			int x = r.x.trunc() - g_engine_context->viewport->scrollx;
			int y = r.y.trunc();
			// what to do if it's near the wraparound? just draw three times?
			renderer->renderCreaturesImage(r.sprite, r.frame(), x, y - g_engine_context->viewport->scrolly);
			renderer->renderCreaturesImage(r.sprite, r.frame(), x - CREATURES1_WORLD_WIDTH, y - g_engine_context->viewport->scrolly);
			renderer->renderCreaturesImage(r.sprite, r.frame(), x + CREATURES1_WORLD_WIDTH, y - g_engine_context->viewport->scrolly);
		}
		// // draw rooms
		for (auto& room : g_engine_context->map->rooms) {
			// what to do if it's near the wraparound? just draw three times?
			for (auto offset : {-CREATURES1_WORLD_WIDTH, 0, CREATURES1_WORLD_WIDTH}) {
				auto left = room.left - g_engine_context->viewport->scrollx + offset;
				auto top = room.top - g_engine_context->viewport->scrolly;
				auto right = room.right - g_engine_context->viewport->scrollx + offset;
				auto bottom = room.bottom - g_engine_context->viewport->scrolly;
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

		// present
		g_engine_context->backend->drawDone();
	}

	return 0;
}