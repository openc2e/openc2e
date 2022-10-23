// dear imgui: Renderer Backend for SDL_Renderer
// (Requires: SDL 2.0.17+)

// Important to understand: SDL_Renderer is an _optional_ component of SDL.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.
// If your application will want to render any non trivial amount of graphics other than UI,
// please be aware that SDL_Renderer offers a limited graphic API to the end-user and it might
// be difficult to step out of those boundaries.
// However, we understand it is a convenient choice to get an app started easily.

// Implemented features:
//  [X] Renderer: User texture binding. Use 'SDL_Texture*' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

struct SDL_Renderer;
struct SDL_Texture;

IMGUI_IMPL_API bool     ImGui_ImplSDLRenderer_Init(SDL_Renderer* renderer);
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_RenderDrawData(ImDrawData* draw_data);

// Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplSDLRenderer_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplSDLRenderer_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_DestroyDeviceObjects();

// SDL_Renderer data
struct ImGui_ImplSDLRenderer_Data
{
    SDL_Renderer*   SDLRenderer;
    SDL_Texture*    FontTexture;
    ImGui_ImplSDLRenderer_Data() { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplSDLRenderer_Data* ImGui_ImplSDLRenderer_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplSDLRenderer_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}