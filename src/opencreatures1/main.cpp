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
#include "PointerManager.h"
#include "Renderable.h"
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


// SDL tries stealing main on some platforms, which we don't want.
#undef main

void load_everything() {
	// set up global objects
	g_engine_context.backend = std::make_shared<SDLBackend>();
	g_engine_context.audio_backend = SDLMixerBackend::getInstance();
	g_engine_context.audio_backend->init(); // TODO: initialized early so SFC sounds can start.. is this right?

	// load palette
	g_engine_context.images->load_default_palette();

	// load Eden.sfc
	auto eden_sfc_path = g_engine_context.paths->find_path(PATH_TYPE_MAIN, "Eden.sfc");
	if (eden_sfc_path.empty()) {
		fmt::print(stderr, "* Error: Couldn't find Eden.sfc\n");
		exit(1);
	}
	fmt::print("* Found Eden.sfc: {}\n", repr(eden_sfc_path));
	auto sfc = sfc::read_sfc_v1_file(eden_sfc_path);

	// load world data
	SFCLoader loader(sfc);
	loader.load_everything();

	// fire init scripts
	for (auto& o : *g_engine_context.objects) {
		g_engine_context.events->queue_script(o, o, SCRIPT_INITIALIZE);
	}
}

void draw_everything() {
	auto renderer = g_engine_context.backend->getMainRenderTarget();

	// draw world (twice, to handle wraparound)
	renderer->renderCreaturesImage(g_engine_context.map->background, 0, -g_engine_context.viewport->scrollx, -g_engine_context.viewport->scrolly);
	renderer->renderCreaturesImage(g_engine_context.map->background, 0, -g_engine_context.viewport->scrollx + CREATURES1_WORLD_WIDTH, -g_engine_context.viewport->scrolly);

	// draw entities
	std::vector<Renderable*> renderables;
	for (auto& o : *g_engine_context.objects) {
		for (int32_t partno = 0; true; ++partno) {
			auto* r = o->get_renderable_for_part(partno);
			if (!r) {
				break;
			}
			renderables.push_back(r);
		}
	}
	std::stable_sort(renderables.begin(), renderables.end(), [](auto* left, auto* right) {
		return left->z < right->z;
	});
	for (auto* r : renderables) {
		int x = r->x.trunc() - g_engine_context.viewport->scrollx;
		int y = r->y.trunc();
		// what to do if it's near the wraparound? just draw three times?
		renderer->renderCreaturesImage(r->sprite, numeric_cast<uint32_t>(r->frame()), x, y - g_engine_context.viewport->scrolly);
		renderer->renderCreaturesImage(r->sprite, numeric_cast<uint32_t>(r->frame()), x - CREATURES1_WORLD_WIDTH, y - g_engine_context.viewport->scrolly);
		renderer->renderCreaturesImage(r->sprite, numeric_cast<uint32_t>(r->frame()), x + CREATURES1_WORLD_WIDTH, y - g_engine_context.viewport->scrolly);
	}

	// draw rooms
	for (auto& room : g_engine_context.map->rooms) {
		// what to do if it's near the wraparound? just draw three times?
		for (auto offset : {-CREATURES1_WORLD_WIDTH, 0, CREATURES1_WORLD_WIDTH}) {
			auto left = room.left - g_engine_context.viewport->scrollx + offset;
			auto top = room.top - g_engine_context.viewport->scrolly;
			auto right = room.right - g_engine_context.viewport->scrollx + offset;
			auto bottom = room.bottom - g_engine_context.viewport->scrolly;
			uint32_t color;
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
}

void update_animations() {
	for (auto& o : *g_engine_context.objects) {
		for (int32_t partno = 0; true; ++partno) {
			auto* r = o->get_renderable_for_part(partno);
			if (!r) {
				break;
			}

			if (!r->has_animation) {
				continue;
			}

			if (r->animation_frame >= r->animation_string.size()) {
				// already done
				// TODO: are we on the correct frame already?
				// TODO: clear animation?
				r->has_animation = false;
				r->animation_string = {};
				r->animation_frame = 0;
				continue;
			}

			// some objects in Eden.sfc start at the 'R' character, so set frame
			// before incrementing.
			// TODO: assert isdigit
			if (r->animation_string[r->animation_frame] == 'R') {
				r->animation_frame = 0;
			}
			r->sprite_index = r->animation_string[r->animation_frame] - '0';
			r->animation_frame += 1;
		}
	}
}

void update_everything() {
	// these should update as often as possible, regardless of ticks
	g_engine_context.music->update();

	// some things can update only every "tick" - 1/10sec
	static std::chrono::time_point<std::chrono::steady_clock> time_of_last_tick{};
	auto time_since_last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - time_of_last_tick)
									.count();
	if (time_since_last_tick >= 100) {
		time_of_last_tick = std::chrono::steady_clock::now();
		g_engine_context.viewport->tick();
		g_engine_context.objects->tick();
		g_engine_context.timers->tick();
		g_engine_context.macros->tick();
		// animations tick after CAOS runs, otherwise the OVER command is too fast
		update_animations();
	}

	// these should update as often as possible, regardless of ticks
	// update pointer after viewport
	g_engine_context.pointer->update();
}

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
	g_engine_context.paths->set_main_directory(datapath);
	load_everything();

	// run loop
	g_engine_context.backend->init("opencreatures1", OPENC2E_DEFAULT_WIDTH, OPENC2E_DEFAULT_HEIGHT);
	while (true) {
		g_engine_context.backend->waitForNextDraw();

		// handle ui events
		BackendEvent event;
		bool should_quit = false;
		while (g_engine_context.backend->pollEvent(event)) {
			g_engine_context.viewport->handle_event(event);
			g_engine_context.pointer->handle_event(event);
			if (event.type == eventquit) {
				should_quit = true;
			}
		}
		if (should_quit) {
			break;
		}

		// update world
		update_everything();

		// draw
		draw_everything();

		// present
		g_engine_context.backend->drawDone();
	}

	return 0;
}