#include "AgentInjector.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ghc/filesystem.hpp>

#include "Backend.h"
#include "CobManager.h"
#include "Texture.h"
#include "Engine.h"
#include "ImGuiUtils.h"
#include "World.h"

namespace Openc2eImgui {

static bool s_read_agents = false;
static Texture s_cob_texture;
static size_t s_object_selected_index = 0;

static CobManager s_object_injector;

static bool s_agent_injector_open = false;

static void DrawAgentPreview(Texture& texture) {
	ImVec2 size = ImVec2(std::max<float>(300, ImGui::GetWindowContentRegionWidth()), 211);
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	ImVec2 p = ImGui::GetCursorScreenPos();
	// should be approx 300 x 170. Alima.bmp is 312 x 211
	ImGuiIO& io = ImGui::GetIO();
	
	ImRect bb(p, p + size);
	drawlist->AddRectFilled(
		bb.Min,
		bb.Max,
		IM_COL32(0, 0, 0, 255),
		ImGui::GetStyle().FrameRounding
	);

	if (texture) {
		int w = texture.width, h = texture.height;
		// w *= io.DisplayFramebufferScale.x;
		// h *= io.DisplayFramebufferScale.y;
		ImVec2 tex_p = p + size / 2 - ImVec2(w, h) / 2;
		drawlist->AddImage(texture.as<ImTextureID>(), tex_p, tex_p + ImVec2(w, h));
	}

	ImGui::Dummy(size);
}

bool IsAgentInjectorEnabled() {
	return engine.version == 1 || engine.version == 2;
}

void SetAgentInjectorOpen(bool is_open) {
  s_agent_injector_open = is_open;
}

void DrawAgentInjector() {
	if (ImGuiUtils::BeginWindow("Agent Injector", &s_agent_injector_open)) {
		if (!s_read_agents) {
			s_object_injector.update();
			s_read_agents = true;
		}

		{
			ImGui::BeginChild("Objects", ImVec2(300, 100), true);
			for (size_t i = 0; i < s_object_injector.objects.size(); ++i) {
				auto obj = s_object_injector.objects[i];
				if (ImGui::Selectable(obj.name.c_str(), i == s_object_selected_index)) {
					s_cob_texture = {};
					s_object_selected_index = i;
				}
			}
			ImGui::EndChild();
		}

		if (!s_cob_texture && s_object_selected_index < s_object_injector.objects.size()) {
				s_cob_texture = engine.backend->createTexture(
					s_object_injector.getPicture(s_object_injector.objects[s_object_selected_index])
				);
		}
		DrawAgentPreview(s_cob_texture);

		if (ImGui::Button("Add Object") && s_object_selected_index < s_object_injector.objects.size()) {
			s_object_injector.inject(s_object_injector.objects[s_object_selected_index]);
		}
		ImGui::SameLine();
		if (s_object_selected_index < s_object_injector.objects.size() && s_object_injector.objects[s_object_selected_index].is_removable) {
			if (ImGui::Button("Remove Object")) {
				s_object_injector.remove(s_object_injector.objects[s_object_selected_index]);
			}
		} else {
			ImGuiUtils::DisabledButton("Remove Object");
		}
		
		ImGuiUtils::EndWindow();
	}
}

} // namespace Openc2eImgui