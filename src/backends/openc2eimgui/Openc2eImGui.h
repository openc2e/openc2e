#pragma once

union SDL_Event;
struct SDL_Window;

namespace Openc2eImGui {

void Init(SDL_Window *window);
void Update(SDL_Window *window);
int GetViewportOffsetTop();
int GetViewportOffsetBottom();
void Render();
bool ConsumeEvent(const SDL_Event &event);

} // namespace Openc2eImgui