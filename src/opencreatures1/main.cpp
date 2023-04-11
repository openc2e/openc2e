#include "C1MusicManager.h"
#include "C1SoundManager.h"
#include "EngineContext.h"
#include "ImageManager.h"
#include "MacroCommands.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "MessageManager.h"
#include "ObjectManager.h"
#include "PathManager.h"
#include "PointerManager.h"
#include "Renderable.h"
#include "SFCSerialization.h"
#include "Scriptorium.h"
#include "ViewportManager.h"
#include "common/Repr.h"
#include "common/backend/Keycodes.h"
#include "common/backtrace.h"
#include "common/render/RenderSystem.h"
#include "sdlbackend/SDLBackend.h"
#include "sdlbackend/SDLMixerBackend.h"

#include <imgui.h>

#ifdef _WIN32
#include "common/WindowsRegistry.h"
#endif

#include <SDL.h>
#include <chrono>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>
#include <math.h>

namespace fs = ghc::filesystem;


// SDL tries stealing main on some platforms, which we don't want.
#undef main

void load_everything() {
	// set up global objects
	set_backend(SDLBackend::get_instance());
	get_backend()->init("opencreatures1", OPENC2E_DEFAULT_WIDTH, OPENC2E_DEFAULT_HEIGHT);

	set_audio_backend(SDLMixerBackend::get_instance());
	get_audio_backend()->init(); // TODO: initialized early so SFC sounds can start.. is this right?

	get_rendersystem()->world_set_wrap_width(CREATURES1_WORLD_WIDTH);
	g_engine_context.sounds->set_listener_world_wrap_width(CREATURES1_WORLD_WIDTH);

	// load palette
	g_engine_context.images->load_default_palette();

	// load World.openc1.sfc/World.sfc/Eden.sfc
	auto sfc_path = g_engine_context.paths->find_path(PATH_TYPE_MAIN, "World.openc1.sfc");
	if (sfc_path.empty()) {
		fmt::print(stderr, "* Couldn't find World.openc1.sfc\n");
		sfc_path = g_engine_context.paths->find_path(PATH_TYPE_MAIN, "World.sfc");
	}
	if (sfc_path.empty()) {
		fmt::print(stderr, "* Couldn't find World.sfc\n");
		sfc_path = g_engine_context.paths->find_path(PATH_TYPE_MAIN, "Eden.sfc");
	}
	if (sfc_path.empty()) {
		fmt::print(stderr, "* Error: Couldn't find Eden.sfc\n");
		exit(1);
	}
	fmt::print("* Loading world: {}\n", repr(sfc_path));
	auto sfc = sfc::read_sfc_v1_file(sfc_path);

	// load world data
	sfc_load_everything(sfc);

	// fire init scripts
	for (auto& o : *g_engine_context.objects) {
		g_engine_context.macros->queue_script(o.get(), o.get(), SCRIPT_INITIALIZE);
	}
}

void update_everything() {
	// these should update as often as possible, regardless of ticks
	g_engine_context.music->update();

	// some things can update only every "tick" - 1/10sec
	using namespace std::chrono;
	static time_point<steady_clock> time_of_last_frame{steady_clock::now()};
	static time_point<steady_clock> time_of_last_tick{steady_clock::now()};
	auto now = steady_clock::now();
	auto time_since_last_frame = duration_cast<milliseconds>(now - time_of_last_frame).count();
	auto time_since_last_tick = duration_cast<milliseconds>(now - time_of_last_tick).count();
	time_of_last_frame = now;
	if (time_since_last_tick >= 100 - time_since_last_frame / 2) {
		// printf("time_since_last_frame %lli time_since_last_tick %lli (%+lli)\n",
		// 	time_since_last_frame,
		// 	time_since_last_tick,
		// 	time_since_last_tick - 100);
		time_of_last_tick = now;
		if (time_since_last_tick < 100) {
			time_of_last_tick += milliseconds(100 - time_since_last_tick);
		} else if (time_since_last_tick > 100) {
			time_of_last_tick -= milliseconds(time_since_last_tick % 100);
		}
		g_engine_context.viewport->tick();
		g_engine_context.objects->tick();
		g_engine_context.macros->tick();
		g_engine_context.messages->tick();
	}

	// these should update as often as possible, regardless of ticks
	// update pointer after viewport
	g_engine_context.pointer->update();
}

int32_t get_fps() {
	static std::chrono::steady_clock::time_point time_of_last_render{};
	static std::array<std::chrono::milliseconds::rep, 60> rendertimes{};
	static size_t rendertimeptr = 0;

	auto n = std::chrono::steady_clock::now();
	rendertimes[rendertimeptr++] =
		std::chrono::duration_cast<std::chrono::milliseconds>(n - time_of_last_render).count();
	if (rendertimeptr == rendertimes.size()) {
		rendertimeptr = 0;
	}
	time_of_last_render = n;
	float avgtime = 0;
	for (auto t : rendertimes) {
		avgtime += t;
	}
	avgtime /= rendertimes.size();
	return static_cast<int32_t>(std::round(1000 / avgtime));
}

void draw_imgui_bottombar() {
	// draw status bar on bottom of window
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y), 0, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 0));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	if (ImGui::Begin("BottomBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
		ImGui::Text("opencreatures1 - %i fps", get_fps());
	}
	ImGui::PopStyleVar(ImGuiStyleVar_WindowBorderSize);
}

void draw_imgui_rightclick_menu() {
	if (ImGui::GetIO().MouseClicked[0] && ImGui::GetIO().KeyMods & ImGuiModFlags_Super) {
		ImGui::OpenPopup("Menu");
	}
	if (ImGui::BeginPopup("Menu")) {
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("Quit", nullptr, false, false);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			ImGui::MenuItem("Show Map", nullptr, false, false);
			ImGui::MenuItem("Create a new (debug) Norn", nullptr, false, false);
			ImGui::MenuItem("Create a random egg", nullptr, false, false);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools")) {
			ImGui::MenuItem("Hatchery", nullptr, false, false);
			ImGui::MenuItem("Agent Injector", nullptr, false, false);
			ImGui::MenuItem("Brain Viewer", nullptr, false, false);
			ImGui::MenuItem("Creature Grapher", nullptr, false, false);
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}

extern "C" int main(int argc, char** argv) {
	install_backtrace_printer();

	std::string datapath;
	if (argc != 2) {
#ifdef _WIN32
		datapath = registry_get_string_value(REGISTRY_HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Gameware Development\\Creatures 1\\1.0", "Main Directory");
		if (datapath.empty()) {
			fmt::print(stderr, "Couldn't find Creatures 1 registry key\n");
			fmt::print(stderr, "Usage: {} [path-to-creatures1-data]\n", argv[0]);
			return 1;
		}
#else
		fmt::print(stderr, "Usage: {} path-to-creatures1-data\n", argv[0]);
		return 1;
#endif
	}

	if (datapath.empty()) {
		datapath = argv[1];
	}
	if (!fs::exists(datapath)) {
		fmt::print(stderr, "* Error: Data path {} does not exist\n", repr(datapath));
		return 1;
	}

	fmt::print("* Creatures 1 Data: {}\n", repr(datapath));
	g_engine_context.paths->set_main_directory(datapath);
	load_everything();

	// run loop
	get_backend()->run([&] {
		// handle ui events
		BackendEvent event;
		while (get_backend()->pollEvent(event)) {
			g_engine_context.viewport->handle_event(event);
			g_engine_context.pointer->handle_event(event);
		}

		// imgui
		draw_imgui_bottombar();
		draw_imgui_rightclick_menu();

		// update world
		update_everything();

		// draw
		get_rendersystem()->draw();
		return true;
	});

	// save world data
	auto sfc = sfc_dump_everything();
	auto out = g_engine_context.paths->ofstream(PATH_TYPE_MAIN, "World.openc1.sfc");
	write_sfc_v1_file(out, sfc);
	fmt::print("* Saved world to: World.openc1.sfc\n");

	// explicitly destroy game data
	// C1ControlledSounds need to be destroyed before the AudioBackend is destroyed,
	// because they'll try to stop their AudioChannel.
	g_engine_context.reset();

	return 0;
}
