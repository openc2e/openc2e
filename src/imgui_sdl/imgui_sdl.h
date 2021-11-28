#pragma once

#include "SDL.h"

struct ImDrawData;

bool ImGuiSDL_Init(SDL_Window* window);
bool ImGuiSDL_ProcessEvent(const SDL_Event* event);
void ImGuiSDL_NewFrame(SDL_Window* window);
void ImGuiSDL_Shutdown();
void ImGuiSDL_RenderDrawData(ImDrawData* drawData);
