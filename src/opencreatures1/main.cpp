#include "C1MusicManager.h"
#include "C1SoundManager.h"
#include "EngineContext.h"
#include "ImageManager.h"
#include "MacroCommands.h"
#include "MacroManager.h"
#include "MapManager.h"
#include "MessageManager.h"
#include "PathManager.h"
#include "PointerManager.h"
#include "Renderable.h"
#include "SFCSerialization.h"
#include "Scriptorium.h"
#include "ViewportManager.h"
#include "common/OptionsParser.h"
#include "common/StringView.h"
#include "common/backend/Keycodes.h"
#include "common/backtrace.h"
#include "common/io/FileWriter.h"
#include "common/render/RenderSystem.h"
#include "objects/ObjectManager.h"
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
	fmt::print("* Loading world: {:?}\n", sfc_path.string());
	auto sfc = sfc::read_sfc_v1_file(sfc_path);

	// load world data
	sfc_load_everything(sfc);

	// fire init scripts
	for (auto* o : *g_engine_context.objects) {
		g_engine_context.macros->queue_script(o, o, SCRIPT_INITIALIZE);
	}
}

void update_everything() {
	// these should update as often as possible, regardless of ticks
	g_engine_context.music->update();

	// some things can update only every "tick" - usually described as
	// 1/10sec but actually every 90 milliseconds, or 1/11.1̅ sec
	using namespace std::chrono;
	static steady_clock::duration accumulator{0};
	static time_point<steady_clock> prev_frame_time{steady_clock::now()};
	auto now = steady_clock::now();
	accumulator += (now - prev_frame_time);
	prev_frame_time = now;

	while (accumulator >= milliseconds(90)) {
		if (accumulator >= milliseconds(180)) {
			printf("multiple ticks, accumulator %lli ms\n", duration_cast<milliseconds>(accumulator).count());
		}

		accumulator -= milliseconds(90);

		g_engine_context.viewport->tick();
		g_engine_context.objects->tick();
		g_engine_context.macros->tick();
		g_engine_context.messages->tick();

		static auto prev_second = duration_cast<seconds>(steady_clock::now().time_since_epoch());
		static uint64_t ticks = 0;
		auto current_second = duration_cast<seconds>(steady_clock::now().time_since_epoch());
		if (current_second != prev_second) {
			prev_second = current_second;
			printf("ticks this second %lli\n", ticks);
			ticks = 0;
		}
		ticks++;
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

	bool no_save = false;
	std::string datapath;
	auto opts = OptionsParser{}
					.opt("--no-save", &no_save)
					.opt("datapath", &datapath)
					.parse(argc, argv);
#ifdef _WIN32
	if (datapath.empty()) {
		datapath = registry_get_string_value(REGISTRY_HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Gameware Development\\Creatures 1\\1.0", "Main Directory");
		if (datapath.empty()) {
			fmt::print(stderr, "Couldn't find Creatures 1 registry key\n");
		}
	}
#endif
	if (datapath.empty()) {
		opts.print_usage_and_fail();
	}

	if (!fs::exists(datapath)) {
		fmt::print(stderr, "* Error: Data path {:?} does not exist\n", datapath);
		return 1;
	}

	fmt::print("* Creatures 1 Data: {:?}\n", datapath);
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
	if (no_save) {
		fmt::print("* Not saving\n");
	} else {
		auto sfc = sfc_dump_everything();
		auto out = g_engine_context.paths->create_file(PATH_TYPE_MAIN, "World.openc1.sfc");
		write_sfc_v1_file(out, sfc);
		fmt::print("* Saved world to: World.openc1.sfc\n");
	}

	// explicitly destroy game data
	// C1ControlledSounds need to be destroyed before the AudioBackend is destroyed,
	// because they'll try to stop their AudioChannel.
	g_engine_context.reset();

	return 0;
}
