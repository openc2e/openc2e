project(imgui_sdl)

add_library(imgui_sdl imgui_sdl.cpp)
target_include_directories(imgui_sdl PUBLIC .)
target_link_libraries(imgui_sdl PUBLIC
	imgui::imgui
	SDL2::SDL2)
target_compile_features(imgui_sdl PRIVATE cxx_std_14)
add_library(imgui_sdl::imgui_sdl ALIAS imgui_sdl)
