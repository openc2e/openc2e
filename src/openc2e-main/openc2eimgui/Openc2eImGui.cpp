#include "Openc2eImGui.h"

#include "AgentInjector.h"
#include "BrainViewer.h"
#include "C1ToolBar.h"
#include "C2StatusBar.h"
#include "C2ToolBar.h"
#include "CreatureGrapher.h"
#include "Engine.h"
#include "Hatchery.h"
#include "MainMenu.h"
#include "common/Exception.h"
#include "imgui_sdl/imgui_sdl.h"

#include <SDL.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace Openc2eImGui {

void Init(SDL_Window* window) {
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.IniFilename = nullptr; // don't save settings

		// {
		// 	io.Fonts->Clear();
		// 	ImFontConfig cfg;
		// 	cfg.SizePixels = 26;
		// 	io.Fonts->AddFontDefault(&cfg);
		// 	io.Fonts->Build();
		// 	io.Fonts->AddFontFromFileTTF("MS Sans Serif.ttf", 14);
		// 	io.Fonts->Build();
		// }

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 0;
		style.WindowRounding = 0;
		// style.Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1.0);
		// style.Colors[ImGuiCol_WindowBg] = ImVec4(198 / 255.0, 198 / 255.0, 198 / 255.0, 1.0);
		// style.Colors[ImGuiCol_PopupBg] = ImVec4(224 / 255.0, 224 / 255.0, 224 / 255.0, 1.0);

		// ImGui::GetStyle().ScaleAllSizes(2);

		if (!ImGuiSDL_Init(window)) {
			throw Exception("Couldn't initialize ImGui - are you using SDL's OpenGL backend?");
		}
	}
}

void Update(SDL_Window* window) {
	ImGuiSDL_NewFrame(window);
	ImGui::NewFrame();

	if (engine.version == 1) {
		Openc2eImgui::DrawC1ToolBar();
	} else if (engine.version == 2) {
		Openc2eImgui::DrawC2Toolbar();
		Openc2eImgui::DrawC2StatusBar();
	}
	if (ImGui::GetIO().MouseClicked[0] && ImGui::GetIO().KeyMods & ImGuiKeyModFlags_Super) {
		ImGui::OpenPopup("Menu");
	}
	if (ImGui::BeginPopup("Menu")) {
		Openc2eImgui::DrawMainMenu();
		ImGui::EndPopup();
	}
	Openc2eImgui::DrawAgentInjector();
	Openc2eImgui::DrawBrainViewer();
	Openc2eImgui::DrawCreatureGrapher();
	Openc2eImgui::DrawHatchery();
}

int GetViewportOffsetTop() {
	if (engine.version == 1) {
		return Openc2eImgui::GetC1ToolBarHeight();
	} else if (engine.version == 2) {
		return Openc2eImgui::GetC2ToolBarHeight();
	} else {
		return 0;
	}
}

int GetViewportOffsetBottom() {
	if (engine.version == 2) {
		return Openc2eImgui::GetC2StatusBarHeight();
	} else {
		return 0;
	}
}

void Render() {
	ImGui::Render();
	ImGuiSDL_RenderDrawData(ImGui::GetDrawData());
}

bool ConsumeEvent(const SDL_Event& event) {
	ImGuiSDL_ProcessEvent(&event);
	if (ImGui::GetIO().WantCaptureMouse && (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL)) {
		return true;
	}
	if (ImGui::GetIO().WantCaptureKeyboard && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_TEXTINPUT)) {
		return true;
	}

	return false;
}

} // namespace Openc2eImGui