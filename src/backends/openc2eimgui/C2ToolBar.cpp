#include "C2ToolBar.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Backend.h"
#include "Engine.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/peFile.h"
#include "AgentInjector.h"
#include "Hatchery.h"
#include "MainMenu.h"
#include "ImGuiUtils.h"
#include "World.h"

using namespace ImGuiUtils;

namespace Openc2eImgui {

static int s_last_c2toolbar_height = 0;

int GetC2ToolBarHeight() {
  return s_last_c2toolbar_height;
}

void DrawC2Toolbar() {
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 0));
  
  if (ImGui::Begin("MainMenuBar", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
    if (ImGui::BeginMenuBar()) {  
      DrawMainMenu();
      ImGui::EndMenuBar();
    }
  
    static Texture s_stdicons_tex;
    if (!s_stdicons_tex) {
      s_stdicons_tex = GetTextureFromExeFileWithTransparentTopLeft(0xe3);
    }
    
    TextureRect icon_next{s_stdicons_tex, 0, 0, 22, 21};
    TextureRect icon_eyeview{s_stdicons_tex, 22, 0, 22, 21};
    TextureRect icon_track{s_stdicons_tex, 44, 0, 22, 21};
    TextureRect icon_halo{s_stdicons_tex, 66, 0, 22, 21};
    TextureRect icon_play{s_stdicons_tex, 88, 0, 22, 21};
    TextureRect icon_pause{s_stdicons_tex, 110, 0, 22, 21};
    TextureRect icon_help{s_stdicons_tex, 132, 0, 22, 21};
    TextureRect icon_web{s_stdicons_tex, 154, 0, 22, 21};
    
    static Texture s_handicons_tex;
    if (!s_handicons_tex) {
      s_handicons_tex = GetTextureFromExeFileWithTransparentTopLeft(0xe4);
    }
    
    TextureRect icon_invisible{s_handicons_tex, 0, 0, 22, 21};
    TextureRect icon_teach{s_handicons_tex, 22, 0, 22, 21};
    TextureRect icon_push{s_handicons_tex, 44, 0, 22, 21};
    
    ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
    
    ImageButton(icon_next, false);
    ImageButton(icon_eyeview, false);
    ImageButton(icon_track, false);
    ImageButton(icon_halo, false);
    ImGui::Separator();
    if (ImageButton(icon_play, world.paused)) {
      world.paused = false;
    };
    if (ImageButton(icon_pause, !world.paused)) {
      world.paused = true;
    }
    ImGui::Separator();
    ImageButton(icon_help, false);
    ImageButton(icon_web, false);
    ImGui::Separator();
    ImageButton(icon_invisible, false);
    ImageButton(icon_teach, false);
    ImageButton(icon_push, false);
    
    ImGui::Separator();
    
    static Texture s_appleticons_tex;
    if (!s_appleticons_tex) {
      s_appleticons_tex = GetTextureFromExeFileWithTransparentTopLeft(0xe6);
    }

    TextureRect icon_hatchery{s_appleticons_tex, 0, 0, 22, 21};
    TextureRect icon_unknown_maybe_ecologykit{s_appleticons_tex, 22, 0, 22, 21};
    TextureRect icon_ownerskit{s_appleticons_tex, 44, 0, 22, 21};
    TextureRect icon_healthkit{s_appleticons_tex, 66, 0, 22, 21};
    TextureRect icon_unknown_maybe_sciencekit{s_appleticons_tex, 88, 0, 22, 21};
    TextureRect icon_unknown_maybe_breederskit{s_appleticons_tex, 110, 0, 22, 21};
    TextureRect icon_unknown_maybe_observationkit{s_appleticons_tex, 132, 0, 22, 21};
    TextureRect icon_agent_injector{s_appleticons_tex, 154, 0, 22, 21};
    TextureRect icon_unknown_maybe_historykit{s_appleticons_tex, 176, 0, 22, 21};
    TextureRect icon_graveyard{s_appleticons_tex, 198, 0, 22, 21};
    TextureRect icon_unknown_globe{s_appleticons_tex, 220, 0, 22, 21};
    TextureRect icon_unknown_magnifyingglass{s_appleticons_tex, 242, 0, 22, 21};
    TextureRect icon_unknown_maybe_neurosciencekit{s_appleticons_tex, 264, 0, 22, 21};
    
    if (ImageButton(icon_hatchery, IsHatcheryEnabled())) {
      SetHatcheryOpen(true);
    };
    ImageButton(icon_ownerskit, false);
    ImageButton(icon_healthkit, false);
    ImageButton(icon_graveyard, false);
    ImGui::Separator();
    ImageButton(icon_unknown_maybe_breederskit, false);
    ImageButton(icon_unknown_maybe_sciencekit, false);
    ImageButton(icon_unknown_maybe_neurosciencekit, false);
    ImageButton(icon_unknown_maybe_observationkit, false);
    ImGui::Separator();
    if (ImageButton(icon_agent_injector, IsAgentInjectorEnabled())) {
      SetAgentInjectorOpen(true);
    }
    ImageButton(icon_unknown_maybe_historykit, false);
    ImageButton(icon_unknown_maybe_ecologykit, false);
    
    ImGui::NewLine();
    
    static Texture s_favtoolbaricons_tex;
    if (!s_favtoolbaricons_tex) {
      s_favtoolbaricons_tex = GetTextureFromExeFileWithTransparentTopLeft(0xe5);
    }
    TextureRect icon_say{s_favtoolbaricons_tex, 0, 0, 22, 21};
    TextureRect icon_go{s_favtoolbaricons_tex, 22, 0, 22, 21};
    
    // ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
    ImGui::Text("Speech History:");
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("##Speech History:", "")) {
      ImGui::EndCombo();
    }
    ImageButton(icon_say);
    ImGui::Separator();
    
    ImGui::Text("Places:");
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("##Places:", "The Incubator")) {
      ImGui::EndCombo();
    }
    ImageButton(icon_go);

    s_last_c2toolbar_height = ImGui::GetWindowSize().y;
    ImGui::End();
  }
}

} // namespace Openc2eImgui