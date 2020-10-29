#include "Hatchery.h"

/*
  C1 hatchery resources:
  hatchery.bmp and htchmask.bmp used for the background/foreground
  SCAN*.BMP and hdsk.wav used for egg disk animation
  EGG*.BMP and hegg.wav used for egg movement animation
  FAN*.BMP and hfan.wav used for the fan animation
  lightoff.bmp and hlgt.wav used for the light flickering
  GENSPIN.BMP, and hmle.wav/hfml.wav used for male/female animation (male.bmp and female.bmp also present)

  C2's Omelette.s16:
  * frames 0-16 are eggs - 11-16 are masked with black
  * frame 17 is an egg shadow
  * frames 22-37 is a rotating questionmark anim
  * frames 38-53 (female) and 54-69 are (male) are rotating gender symbol anims
  *
  * frame 18 and 19 are sides of the hatchery
  * frame 20 is the grabber
  * frame 21 is the middle bit of the side
*/


#include <array>
#include <imgui.h>
#include <imgui_internal.h>

#include "Engine.h"
#include "ImGuiUtils.h"
#include "World.h"
#include "fileformats/bmpImage.h"
#include "fileformats/c16Image.h"

using namespace ImGuiUtils;

namespace Openc2eImgui {

static bool s_hatchery_open = false;
static bool s_hatchery_loaded = false;

static Texture s_hatcherybackground;
static Texture s_hatcherymask;
static std::vector<Texture> s_omelette;

struct C1EggInfo {
  std::string name;
  Texture texture;
  int x;
  int y;
};

std::array<C1EggInfo, 6> s_c1eggs = {{
    {"hatchery/EGG0.bmp", {}, 50, 150},
    {"hatchery/EGG1.bmp", {}, 80, 145},
    {"hatchery/EGG2.bmp", {}, 110, 150},
    {"hatchery/EGG3.bmp", {}, 140, 140},
    {"hatchery/EGG4.bmp", {}, 170, 155},
    {"hatchery/EGG5.bmp", {}, 200, 150},
}};

struct C2EggInfo {
  int frame;
  int x;
  int y;
};

std::array<C2EggInfo, 10> s_c2eggs;

void LoadHatchery() {
  if (engine.version == 1) {
    s_hatcherybackground = LoadImageWithTransparentTopLeft("hatchery/hatchery.bmp")[0];
    s_hatcherymask = LoadImageWithTransparentTopLeft("hatchery/htchmask.bmp")[0];

    for (auto &e : s_c1eggs) {
      e.texture = LoadImageWithTransparentTopLeft(e.name)[0];
    }
  } else if (engine.version == 2) {
    s_hatcherybackground = LoadImageWithTransparentTopLeft("Applet Data/Hatchery.bmp")[0];
    s_omelette = LoadImageWithTransparentTopLeft("Applet Data/Omelette.s16");

    for (size_t i = 0; i < 10; i++) {
      // Place eggs from 80-240 horizontally, 110-230 vertically
      int x = 0, y = 0, tries = 0;
      while (x == 0 && tries < 50) {
        x = (rand() / (RAND_MAX + 1.0)) * (240 - 80 - s_omelette[i].width) + 80;
        y = (rand() / (RAND_MAX + 1.0)) * (230 - 110 - s_omelette[i].height) + 110;
        for (unsigned int j = 0; j < i; j++) {
          float xdist = x - s_c2eggs[j].x;
          xdist = xdist * xdist;
          float ydist = y - s_c2eggs[j].y;
          ydist = ydist * ydist;
          if (xdist + ydist < 20 * 20) {
            // less than 20 pixels apart
            // try again
            x = 0;
          }
        }
        tries++; // give up after 50 attempts at placement..
      }
      s_c2eggs[i].frame = i;
      s_c2eggs[i].x = x;
      s_c2eggs[i].y = y;
    }
    std::sort(s_c2eggs.begin(), s_c2eggs.end(), [](const auto &l, const auto &r) { return l.y < r.y; });
  }

  s_hatchery_loaded = true;
}

void UnloadHatchery() {
  s_hatcherybackground = {};
  s_hatcherymask = {};
  s_omelette = {};
  for (auto &e : s_c1eggs) {
    e.texture = {};
  }
  s_hatchery_loaded = false;
}

bool IsHatcheryEnabled() {
  return engine.version == 1 || engine.version == 2;
}

void SetHatcheryOpen(bool value) {
  s_hatchery_open = value;
}

void DrawHatchery() {
  if (!(engine.version == 1 || engine.version == 2)) {
    return;
  }

  if (s_hatchery_open && ImGui::Begin("Hatchery", &s_hatchery_open,
                                      ImGuiWindowFlags_NoScrollbar |
                                          ImGuiWindowFlags_NoScrollWithMouse |
                                          ImGuiWindowFlags_NoCollapse)) {

    if (!s_hatchery_loaded) {
      LoadHatchery();
    }

    if (engine.version == 1) {
      // background
      ImVec2 p = ImGui::GetCursorScreenPos();
      DrawTexture(s_hatcherybackground, p);
      ImGui::Dummy(ImVec2(320, 240));

      /* fan animation */
      for (unsigned int i = 0; i < 4; i++) {
        // TODO
      }

      /* 'off' state for the light */
      // TODO

      /* eggs */
      for (auto &e : s_c1eggs) {
        if (!e.texture)
          continue;
          
        DrawTexture(e.texture, p + ImVec2(e.x, e.y));
      }

      /* mask which goes over the eggs */
      DrawTexture(s_hatcherymask, p + ImVec2(0, 168));

      /* gender marker animation */
      // TODO
    } else if (engine.version == 2) {
      // background
      ImVec2 p = ImGui::GetCursorScreenPos();
      DrawTexture(s_hatcherybackground, p);
      ImGui::Dummy(ImVec2(320, 275));

      // eggs
      for (int i = 0; i < 10; ++i) {
        DrawTexture(s_omelette[s_c2eggs[i].frame], p + ImVec2(s_c2eggs[i].x, s_c2eggs[i].y));
      }
      // TODO: gender markers and animation
      // TODO: place shadows

      // leftsideimg
      DrawTexture(s_omelette[18], p + ImVec2(75, 176), 0.5);
      // rightsideimg
      DrawTexture(s_omelette[19], p + ImVec2(181, 162), 0.5);
      // grabberimg
      DrawTexture(s_omelette[20], p + ImVec2(150, 93));
      // midsideimg
      DrawTexture(s_omelette[21], p + ImVec2(141, 177));
    }

    ImGui::End();
  }
}

} // namespace Openc2eImgui