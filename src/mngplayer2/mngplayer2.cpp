#include "common/Exception.h"
#include "common/Random.h"
#include "common/audio/AudioBackend.h"
#include "common/backtrace.h"
#include "common/endianlove.h"
#include "common/ends_with.h"
#include "common/readfile.h"
#include "common/scope_guard.h"
#include "common/zip.h"
#include "fileformats/mngfile.h"
#include "libmngmusic/MNGMusic.h"
#include "sdlbackend/SDLBackend.h"
#include "sdlbackend/SDLMixerBackend.h"

#include <SDL.h>
#include <chrono>
#include <cstring>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <nfd.h>
#include <stdlib.h>
#include <string>

// SDL tries stealing main on some platforms, which we don't want.
#ifndef _WIN32
#undef main
#endif

namespace fs = ghc::filesystem;

static SDLBackend backend;
static bool should_quit = false;

constexpr const char* ABOUT_TEXT = R"(MNGPlayer2 v1.0.1
Copyright (C) 2021 the openc2e project

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation, either version 2.1 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
License for more details.

This software package also includes some or all of the following libraries:

* Creatures Icons by derk5482
* fmt - https://fmt.dev/
* ghc_filesystem - https://github.com/gulrak/filesystem/
* imgui - https://github.com/ocornut/imgui/
* nativefiledialog - https://github.com/mlabbe/nativefiledialog
* re2c - https://re2c.org/
* SDL2 - https://www.libsdl.org/
* SDL2_mixer - https://www.libsdl.org/projects/SDL_mixer/
)";

struct BoolVector {
  public:
	BoolVector() {}
	BoolVector(size_t size_, bool val_) {
		size = size_;
		data = new bool[size_];
		for (size_t i = 0; i < size_; ++i) {
			data[i] = val_;
		}
	}
	BoolVector(const BoolVector&) = delete;
	BoolVector(BoolVector&&) = delete;
	BoolVector& operator=(const BoolVector&) = delete;
	BoolVector& operator=(BoolVector&& other) {
		if (data) {
			delete[] data;
		}
		size = other.size;
		data = other.data;
		other.size = 0;
		other.data = nullptr;
		return *this;
	}
	~BoolVector() {
		if (data) {
			delete[] data;
		}
	}

	bool& operator[](size_t i) {
		return data[i];
	}

	bool* data = nullptr;
	size_t size = 0;
};

static float clamp(float val, float min, float max) {
	if (val <= min) {
		return min;
	}
	if (val >= max) {
		return max;
	}
	return val;
}

static fs::path replace_extension(fs::path p, std::string ext) {
	p.replace_extension(ext);
	return p;
}

static bool ShowConfirmBox(const std::string& title, const std::string& message) {
	const SDL_MessageBoxButtonData buttons[] = {
		{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No"},
		{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes"},
	};
	const SDL_MessageBoxData messageboxdata = {
		SDL_MESSAGEBOX_INFORMATION,
		backend.window,
		title.c_str(),
		message.c_str(),
		SDL_arraysize(buttons),
		buttons,
		nullptr};
	int buttonid;
	if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
		SDL_Log("error displaying message box");
		return false;
	}
	return buttonid == 1;
}

static struct AppState {
	std::string fullpath;
	std::string filename;
	std::string trackname;
	MNGFile mngfile;
	MNGScript parsed_script;

	bool random_play = true;
	BoolVector random_allowed_track_indices;
	int random_interval = 300;
	char random_interval_buf[6] = "300";
	std::chrono::steady_clock::time_point random_time_of_last_switch = {};

	std::shared_ptr<SDLMixerBackend> audio_backend = nullptr;
	std::unique_ptr<MNGMusic> mng_music;

	float mood = 0.5;
	float threat = 0.5;
	float volume = 1.0;

	enum State {
		STATE_NO_FILE_LOADED,
		STATE_PLAYING,
		STATE_STOPPED
	} state = STATE_NO_FILE_LOADED;

	enum LoadedFileType {
		TYPE_NO_FILE_LOADED,
		TYPE_MNGFILE,
		TYPE_MNGSCRIPT
	} loaded_file_type = TYPE_NO_FILE_LOADED;

	void Update() {
		mng_music->update();

		if (state == STATE_PLAYING && random_play && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - random_time_of_last_switch).count() >= random_interval) {
			playMusic();
		}
	}

	bool canPlayMusic() {
		return state == STATE_STOPPED;
	}

	void playMusic() {
		if (!parsed_script.tracks.size()) {
			return;
		}
		std::vector<std::string> eligible_tracknames;
		for (size_t i = 0; i < parsed_script.tracks.size(); ++i) {
			auto t = parsed_script.tracks[i];
			if (t.name == trackname || !random_allowed_track_indices[i]) {
				continue;
			}
			eligible_tracknames.push_back(t.name);
		}
		if (!eligible_tracknames.size()) {
			for (auto t : parsed_script.tracks) {
				eligible_tracknames.push_back(t.name);
			}
		}
		std::string newtrackname = rand_choice(eligible_tracknames);
		playMusic(newtrackname);
	}

	void playMusic(std::string newtrackname) {
		mng_music->playTrack(&mngfile, newtrackname);
		// TODO: exceptions?
		trackname = newtrackname;
		state = STATE_PLAYING;
		if (random_play) {
			random_time_of_last_switch = std::chrono::steady_clock::now();
		}
	}

	bool canStopMusic() {
		return state == STATE_PLAYING;
	}

	void stopMusic() {
		mng_music->stop();
		trackname = "";
		state = STATE_STOPPED;
	}

	void setRandomInterval(int new_random_interval) {
		random_interval = new_random_interval;
	}

	void openFile() {
		const nfdchar_t* filterList = "mng";
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog(filterList, nullptr, &outPath);

		if (result == NFD_CANCEL) {
			return;
		}
		if (result != NFD_OKAY) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening file", NFD_GetError(), backend.window);
			return;
		}

		auto outpath_guard = make_scope_guard([&] { free(outPath); });

		MNGFile newfile;
		MNGScript newscript;

		try {
			newfile = MNGFile(outPath);
			newscript = mngparse(newfile.script);
		} catch (const std::exception& e) {
			std::string message = std::string("Error opening file: ") + outPath + "\n\n" + e.what();
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening file", message.c_str(), backend.window);
			return;
		}

		stopMusic();

		fullpath = outPath;
		filename = fs::path(outPath).filename();
		mngfile = newfile;
		parsed_script = newscript;
		random_allowed_track_indices = BoolVector(parsed_script.tracks.size(), true);
		loaded_file_type = TYPE_MNGFILE;
	}

	void openScript() {
		const nfdchar_t* filterList = "txt";
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog(filterList, nullptr, &outPath);

		if (result == NFD_CANCEL) {
			return;
		}
		if (result != NFD_OKAY) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening file", NFD_GetError(), backend.window);
			return;
		}

		auto outpath_guard = make_scope_guard([&] { free(outPath); });

		MNGFile newfile;
		try {
			newfile.name = fs::path(outPath).stem();
			newfile.script = readfile(outPath);

			auto sample_names = mngparse(newfile.script).getWaveNames();
			for (size_t i = 0; i < sample_names.size(); ++i) {
				auto sample_filename = (fs::path(outPath).parent_path() / sample_names[i]).string() + ".wav";

				std::ifstream in(sample_filename, std::ios_base::binary);
				if (!in.is_open()) {
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening file", sample_filename.c_str(), backend.window);
					return;
				}
				// in.ignore(16); // skip wav header
				auto data = readfilebinary(in);
				newfile.samples.push_back(shared_array<uint8_t>(data.begin(), data.end()));
				newfile.samplemappings[sample_names[i]] = i;
			}

		} catch (const std::exception& e) {
			std::string message = std::string("Error opening file: ") + outPath + "\n\n" + e.what();
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening file", message.c_str(), backend.window);
			return;
		}

		stopMusic();

		fullpath = outPath;
		filename = fs::path(outPath).filename();
		mngfile = newfile;
		parsed_script = mngparse(newfile.script);
		random_allowed_track_indices = BoolVector(parsed_script.tracks.size(), true);
		loaded_file_type = TYPE_MNGSCRIPT;
	}

	bool canCompile() {
		return loaded_file_type == TYPE_MNGSCRIPT;
	}

	void compile() {
		std::string output_path = replace_extension(fullpath, "mng");
		if (ends_with_ignore_case(output_path, ".mng")) {
			output_path.resize(output_path.size() - 4);
		}

		if (fs::exists(output_path)) {
			std::string message = "Overwrite " + output_path + " ?";
			if (!ShowConfirmBox("Confirm", message.c_str())) {
				return;
			}
		}

		try {
			std::ofstream out(output_path, std::ios_base::binary);
			out.exceptions(std::ofstream::failbit | std::ofstream::badbit);

			const int number_of_samples = mngfile.samples.size();
			write32le(out, number_of_samples);

			const int script_position = 12 + number_of_samples * 8;
			write32le(out, script_position);

			const int script_size = mngfile.script.size();
			write32le(out, script_size);

			int sample_position = script_position + script_size;
			for (auto& s : mngfile.samples) {
				write32le(out, sample_position);
				const int sample_size = s.size() - 16;
				write32le(out, sample_size);
				sample_position += sample_size;
			}

			std::vector<uint8_t> encryptedscript = mngencrypt(mngfile.script);
			out.write((char*)encryptedscript.data(), encryptedscript.size());

			for (auto& s : mngfile.samples) {
				out.write((char*)s.data() + 16, s.size() - 16);
			}

		} catch (const std::exception& e) {
			std::string message = "Error compiling script: " + output_path + "\n\n" + e.what();
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error compiling script", message.c_str(), backend.window);
			return;
		}

		std::string message = "Compiled MNG script to '" + output_path + "'";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Done!", message.c_str(), backend.window);
	}

	bool canDecompile() {
		return loaded_file_type == TYPE_MNGFILE;
	}

	void decompile() {
		fs::path output_directory = fs::path(fullpath).parent_path() / replace_extension(filename, "mng.decompiled");

		if (fs::exists(output_directory)) {
			std::string message = "Overwrite " + output_directory.string() + " ?";
			if (!ShowConfirmBox("Confirm", message.c_str())) {
				return;
			}
		}

		if (!fs::create_directories(output_directory)) {
			if (!fs::is_directory(output_directory)) {
				std::string message = "Couldn't create output directory " + output_directory.string();
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error saving file", message.c_str(), backend.window);
				return;
			}
		}

		try {
			fs::path script_filename = output_directory / replace_extension(filename, "mng.txt");
			std::ofstream out(script_filename, std::ios_base::binary);
			out.exceptions(std::ofstream::failbit | std::ofstream::badbit | std::ofstream::eofbit);
			out.write(mngfile.script.data(), mngfile.script.size());


			for (auto kv : zip(mngfile.getSampleNames(), mngfile.samples)) {
				fs::path sample_filename = (output_directory / kv.first).string() + ".wav";
				std::ofstream out(sample_filename, std::ios_base::binary);
				out.exceptions(std::ofstream::failbit | std::ofstream::badbit | std::ofstream::eofbit);
				out.write((const char*)kv.second.data(), kv.second.size());
			}

		} catch (const std::exception& e) {
			std::string message = "Error decompiling file: " + output_directory.string() + "\n\n" + e.what();
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error decompiling file", message.c_str(), backend.window);
			return;
		}

		std::string message = "Decompiled MNG file to '" + output_directory.string() + "'";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Done!", message.c_str(), backend.window);
	}

	void variablesUpdated() {
		mood = clamp(mood, 0, 1);
		threat = clamp(threat, 0, 1);
		volume = clamp(volume, 0, 1);
		mng_music->setMood(mood);
		mng_music->setThreat(threat);
		mng_music->setVolume(volume);
	}
} app_state;

void TextCentered(const char* text) {
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) * 0.5f);
	ImGui::Text("%s", text);
}

bool ButtonEx(const char* text, bool enabled = true) {
	if (!enabled) {
		ImGui::BeginDisabled();
	}
	bool result = ImGui::Button(text);
	if (!enabled) {
		ImGui::EndDisabled();
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	return result;
}

bool NiceCursorCheckbox(const char* label, bool* val) {
	bool result = ImGui::Checkbox(label, val);
	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	return result;
}

bool SelectableEx(const char* label, bool* selected) {
	bool result = ImGui::Selectable(label, selected);
	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	return result;
}

void DrawImGui() {
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("##mainwindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollWithMouse);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open file...")) {
				app_state.openFile();
			}
			if (ImGui::MenuItem("Open script...")) {
				app_state.openScript();
			}
			// TODO
			// if (ImGui::BeginMenu("Open Recent")) { ImGui::EndMenu(); }
			ImGui::Separator();
			if (ImGui::MenuItem("Compile...", "", false, app_state.canCompile())) {
				app_state.compile();
			}
			if (ImGui::MenuItem("Decompile...", "", false, app_state.canDecompile())) {
				app_state.decompile();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close")) {
				should_quit = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Playback")) {
			if (ImGui::MenuItem("Play", "", false, app_state.canPlayMusic())) {
				app_state.playMusic();
			}
			if (ImGui::MenuItem("Stop", "", false, app_state.canStopMusic())) {
				app_state.stopMusic();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("About MNGPlayer2")) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "About MNGPlayer2", ABOUT_TEXT, backend.window);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::BeginChild("##tracks", ImVec2(-1, 200), true)) {
		for (size_t i = 0; i < app_state.parsed_script.tracks.size(); ++i) {
			auto t = app_state.parsed_script.tracks[i];
			if (!app_state.random_play) {
				ImGui::BeginDisabled();
			}
			ImGui::PushID(i);
			NiceCursorCheckbox("##track-randomplay-eligible", &app_state.random_allowed_track_indices[i]);
			ImGui::PopID();
			if (!app_state.random_play) {
				ImGui::EndDisabled();
			}
			ImGui::SameLine();

			bool selected = t.name == app_state.trackname;
			if (SelectableEx(t.name.c_str(), &selected)) {
				app_state.playMusic(t.name);
			}
		}

		ImGui::EndChild();
	}

	if (ImGui::BeginTable("##randomplaytable", 3)) {
		ImGui::TableSetupColumn("##randomplaytable-left", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("##randomplaytable-center", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("##randomplaytable-right", ImGuiTableColumnFlags_WidthFixed);

		ImGui::TableNextColumn();

		ImGui::Text("Random Play:");
		ImGui::SameLine();
		ImGui::GetCurrentWindow()->DC.CursorPos.y -= 1;
		NiceCursorCheckbox("##randomplay", &app_state.random_play);

		ImGui::TableNextColumn();
		ImGui::TableNextColumn();
		ImGui::GetCurrentWindow()->DC.CursorPos.y -= 1;
		if (!app_state.random_play) {
			ImGui::BeginDisabled();
		}
		ImGui::Text("Interval:");
		ImGui::SameLine();

		if (ImGui::InputText("##interval", app_state.random_interval_buf, sizeof(app_state.random_interval_buf), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::IsItemDeactivatedAfterEdit()) {
			int new_random_interval = std::atoi(app_state.random_interval_buf);
			if (new_random_interval != 0) {
				app_state.setRandomInterval(new_random_interval);
			}
		}
		if (!app_state.random_play) {
			ImGui::EndDisabled();
		}

		ImGui::EndTable();
	}

	if (ImGui::BeginTable("##sliders", 2)) {
		ImGui::TableSetupColumn("##sliders-label", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("##sliders-slider", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextColumn();
		ImGui::Text("Mood:   ");

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::SliderFloat("##mood slider", &app_state.mood, 0, 1.0)) {
			app_state.variablesUpdated();
		}

		ImGui::TableNextColumn();
		ImGui::Text("Threat:   ");

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::SliderFloat("##threat slider", &app_state.threat, 0, 1.0)) {
			app_state.variablesUpdated();
		}

		ImGui::TableNextColumn();
		ImGui::Text("Volume:   ");

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-1);
		if (ImGui::SliderFloat("##volume slider", &app_state.volume, 0, 1.0)) {
			app_state.variablesUpdated();
		}

		ImGui::EndTable();
	}

	ImGui::Dummy(ImVec2(0, 2));

	if (ImGui::BeginTable("##controls", 3)) {
		ImGui::TableSetupColumn("##controls-left", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("##controls-center", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("##controls-right", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextColumn();
		ImGui::TableNextColumn();

		if (ButtonEx("Play", app_state.canPlayMusic())) {
			app_state.playMusic();
		}
		ImGui::SameLine();
		if (ButtonEx("Stop", app_state.canStopMusic())) {
			app_state.stopMusic();
		}
		ImGui::SameLine();
		if (ButtonEx("Open file...")) {
			app_state.openFile();
		}

		ImGui::EndTable();
	}

	TextCentered(app_state.filename.size() ? app_state.filename.c_str() : "No file loaded.");

	ImGui::End();
}

void InitMNGMusic() {
	app_state.audio_backend = SDLMixerBackend::getInstance();
	app_state.audio_backend->init();
	app_state.mng_music = std::make_unique<MNGMusic>(app_state.audio_backend);
	app_state.variablesUpdated();
}

void RunMainLoop() {
	while (true) {
		backend.waitForNextDraw();

		// handle ui events
		BackendEvent event;
		while (backend.pollEvent(event)) {
			if (event.type == eventquit) {
				should_quit = true;
			}
		}
		if (should_quit) {
			break;
		}

		// draw
		app_state.Update();
		DrawImGui();
		backend.drawDone();
	}
}

int main(int, char**) {
	install_backtrace_printer();

	try {
		backend.init("MNGPlayer2", 300, 400);
	} catch (Exception& e) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), backend.window);
	}
	InitMNGMusic();

	RunMainLoop();
	return 0;
}
