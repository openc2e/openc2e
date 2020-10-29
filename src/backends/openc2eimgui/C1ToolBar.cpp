#include "C1ToolBar.h"

#include <imgui.h>
#include "MainMenu.h"

namespace Openc2eImgui {

static int s_last_c1toolbar_height = 0;

int GetC1ToolBarHeight() {
  return s_last_c1toolbar_height;
}

void DrawC1ToolBar() {  
  if (ImGui::BeginMainMenuBar()) {
    Openc2eImgui::DrawMainMenu();
    s_last_c1toolbar_height = ImGui::GetWindowSize().y;
    ImGui::EndMainMenuBar();
  }
}

} // namespace Openc2eImgui