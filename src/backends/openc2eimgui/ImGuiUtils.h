#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Texture.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator+(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x - rhs, lhs.y - rhs); }
static inline ImVec2 operator*(double lhs, const ImVec2& rhs) { return ImVec2(lhs * rhs.x, lhs * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }

namespace ImGuiUtils {

void Image(Texture texture);

Texture GetTextureFromExeFile(uint32_t resource);
Texture GetTextureFromExeFileWithTransparentTopLeft(uint32_t resource);
std::vector<Texture> LoadImageWithTransparentTopLeft(const std::string& name);

bool BeginWindow(const char *title, bool* is_open, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
void EndWindow();

void DrawTexture(Texture texture, ImVec2 p, float transparency = 1.0);

void DisabledButton(const char* text);
bool ImageButton(Texture texture, bool enabled = true);

class TextureRect {
public:
  Texture parent;
  unsigned int x;
  unsigned int y;
  unsigned int w;
  unsigned int h;
};
bool ImageButton(TextureRect texture, bool enabled = true);

} // namespace ImGuiUtils
