project(imgui)

add_library(imgui
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_widgets.cpp
	examples/imgui_impl_sdl.cpp
)
target_include_directories(imgui PUBLIC . examples)
target_link_libraries(imgui PUBLIC SDL2::SDL2)
add_library(imgui::imgui ALIAS imgui)
