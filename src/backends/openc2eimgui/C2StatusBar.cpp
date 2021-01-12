#include "C2StatusBar.h"

#include <array>
#include <imgui.h>
#include <imgui_internal.h>

#include "AgentHelpers.h"
#include "Camera.h"
#include "creatures/CreatureAgent.h"
#include "creatures/oldCreature.h"
#include "Engine.h"
#include "MetaRoom.h"
#include "ImGuiUtils.h"
#include "optional.h"
#include "Room.h"
#include "World.h"

using namespace ImGuiUtils;

namespace Openc2eImgui {

static optional<int> getTemperatureNearSelectedCreature() {
  if (world.selectedcreature) {
    auto room = roomContainingAgent(world.selectedcreature);
    if (room) {
      return room->temp;
    }
  }
  return {};
}

static optional<int> getTemperatureAtCenterOfCamera() {
  auto room = engine.camera->getMetaRoom()->roomAt(engine.camera->getXCentre(), engine.camera->getYCentre());
  if (room) {
    return room->temp;
  }
  return {};
}

static int s_last_c2statusbar_height = 0;

int GetC2StatusBarHeight() {
  return s_last_c2statusbar_height;
}

void DrawC2StatusBar() {
  ImGuiIO& io = ImGui::GetIO();
  
  ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - s_last_c2statusbar_height));
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 0));
  if (ImGui::Begin("Statusbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

    static std::array<Texture, 4> s_season_icons_texs;
    if (!s_season_icons_texs[0]) {
      s_season_icons_texs[0] = GetTextureFromExeFile(0x98);
      s_season_icons_texs[1] = GetTextureFromExeFile(0x99);
      s_season_icons_texs[2] = GetTextureFromExeFile(0x9a);
      s_season_icons_texs[3] = GetTextureFromExeFile(0x9b);
    }
    
    ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
    
    switch (world.season) {
      case 0: ImGui::Text("Spring"); break;
      case 1: ImGui::Text("Summer"); break;
      case 2: ImGui::Text("Autumn"); break;
      case 3: ImGui::Text("Winter"); break;
      default: ImGui::Text("Season %i", world.season); break;
    }
    if (world.season < s_season_icons_texs.size()) {
      ImGuiUtils::Image(s_season_icons_texs[world.season]);
    }
    
    ImGui::Text("Year: %03i", world.year);
    
    static std::array<Texture, 5> s_timeofday_icons_texs;
    if (!s_timeofday_icons_texs[0]) {
      s_timeofday_icons_texs[0] = GetTextureFromExeFile(0xa3);
      s_timeofday_icons_texs[1] = GetTextureFromExeFile(0xc4);
      s_timeofday_icons_texs[2] = GetTextureFromExeFile(0xc2);
      s_timeofday_icons_texs[3] = GetTextureFromExeFile(0xc3);
      s_timeofday_icons_texs[4] = GetTextureFromExeFile(0xc5);
    }
    if (world.timeofday < s_timeofday_icons_texs.size()) {
      ImGuiUtils::Image(s_timeofday_icons_texs[world.timeofday]);
    }
    
    static std::array<Texture, 5> s_temperature_icons_texs;
    if (!s_temperature_icons_texs[0]) {
      s_temperature_icons_texs[0] = GetTextureFromExeFile(0xa4);
      s_temperature_icons_texs[1] = GetTextureFromExeFile(0xa5);
      s_temperature_icons_texs[2] = GetTextureFromExeFile(0xa6);
      s_temperature_icons_texs[3] = GetTextureFromExeFile(0xa7);
      s_temperature_icons_texs[4] = GetTextureFromExeFile(0xa8);
    }
    
    // prefer the room the selected creature is in
    optional<int> temperature = getTemperatureNearSelectedCreature();
    if (!temperature) {
      // then try the room at the center of the camera
      temperature = getTemperatureAtCenterOfCamera();
      if (!temperature) {
        // TODO: c2 seems to try closest room as a last resort?
      }
    }
    if (temperature) {
      int temp = temperature.value();
      unsigned int tempiconid = 4;
      if (temp < 255) {
        tempiconid = temp / 64; // boundaries at 64, 128, 192 and 255
      }
      if (tempiconid < s_temperature_icons_texs.size()) {
        ImGuiUtils::Image(s_temperature_icons_texs[tempiconid]);
      }
      unsigned int temp_celsius = ((temp * 80) / 255) - 15;
      ImGui::Text("%u°", temp_celsius);
      // TODO: fahrenheit calculation + textual version of temperature
      // (0 = -15C = 5F, 48 = 0C = 32F, 255 = 65C = 149F)
    }
    
    static std::array<Texture, 6> s_healthicons_texs;
    if (!s_healthicons_texs[0]) {
      s_healthicons_texs[0] = GetTextureFromExeFileWithTransparentTopLeft(0xe8); // disabled (gray)
      s_healthicons_texs[1] = GetTextureFromExeFileWithTransparentTopLeft(0xc7); // 0/4
      s_healthicons_texs[2] = GetTextureFromExeFileWithTransparentTopLeft(0xac); // 1/4
      s_healthicons_texs[3] = GetTextureFromExeFileWithTransparentTopLeft(0xad); // 2/4
      s_healthicons_texs[4] = GetTextureFromExeFileWithTransparentTopLeft(0xab); // 3/4
      s_healthicons_texs[5] = GetTextureFromExeFileWithTransparentTopLeft(0xaa); // 4/4
    }
    
    static std::array<Texture, 5> s_hearticons_texs;
    if (!s_hearticons_texs[0]) {
      s_hearticons_texs[0] = GetTextureFromExeFileWithTransparentTopLeft(0xe9); // disabled (gray)
      s_hearticons_texs[1] = GetTextureFromExeFileWithTransparentTopLeft(0xae); // large
      s_hearticons_texs[2] = GetTextureFromExeFileWithTransparentTopLeft(0xaf); // medium
      s_hearticons_texs[3] = GetTextureFromExeFileWithTransparentTopLeft(0xb0); // small
      s_hearticons_texs[4] = GetTextureFromExeFileWithTransparentTopLeft(0xe7); // dead (blue)
    }
    
    static int s_last_statusbar_rightside_width = 0;
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - s_last_statusbar_rightside_width);
    const int start_x = ImGui::GetCursorPosX();
    
    ImGui::Text("Health:");
    // TODO: norn health/drive/heartbeat
    if (!world.selectedcreature) {
      ImGuiUtils::Image(s_healthicons_texs[0]);
      ImGuiUtils::Image(s_hearticons_texs[0]);
    } else {
      if (CreatureAgent *ca = dynamic_cast<CreatureAgent *>(world.selectedcreature.get())) {
        oldCreature *c = dynamic_cast<oldCreature *>(ca->getCreature());
        assert(c); // this is c2, after all..
        
        if (c->isAlive()) {
          // this life force indicator shows the level of Glycogen
          // TODO: this does not update instantly w/Glycogen in real
          // c2, so perhaps it's actually another (related) data point?
          unsigned int x = c->getChemical(72) / 64;
          ImGuiUtils::Image(s_healthicons_texs[x+2]); // 2-5 (1-4 bars)
          // TODO: flash indicator when norn is dying

          // TODO: heartbeat animation
          ImGuiUtils::Image(s_hearticons_texs[1]); // 1-3 = beating heart, large to small
        } else {
          // dead
          ImGuiUtils::Image(s_healthicons_texs[1]);
          ImGuiUtils::Image(s_hearticons_texs[4]); // blue heart
        }
        // TODO: drive
      }
    }
    
    s_last_statusbar_rightside_width = ImGui::GetCursorPosX() - start_x;
    
    s_last_c2statusbar_height = ImGui::GetWindowSize().y;
    ImGui::End();
  }
}

} // namespace Openc2eImgui