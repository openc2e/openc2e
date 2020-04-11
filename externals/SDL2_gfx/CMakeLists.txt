project(SDL2_gfx)

add_library(SDL2_gfx
	SDL2_framerate.c
	SDL2_gfxPrimitives.c
	SDL2_imageFilter.c
	SDL2_rotozoom.c
)
target_link_libraries(SDL2_gfx PRIVATE SDL2)
target_include_directories(SDL2_gfx PUBLIC .)