project(imgui)

add_library(imgui
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_tables.cpp
	imgui_widgets.cpp
	backends/imgui_impl_sdl2.cpp
	backends/imgui_impl_sdlrenderer2.cpp
)
target_include_directories(imgui PUBLIC . backends)
target_link_libraries(imgui PUBLIC SDL2::SDL2)
set_property(TARGET imgui PROPERTY CXX_STANDARD 14)
add_library(imgui::imgui ALIAS imgui)
