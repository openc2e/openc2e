include(FindPackageHandleStandardArgs)

if(EMSCRIPTEN)
    add_library(sdl2_mixer INTERFACE)
    add_library(SDL2::Mixer ALIAS sdl2_mixer)
	target_compile_options(sdl2_mixer INTERFACE "SHELL:-s USE_SDL_MIXER=2")
	target_link_libraries(sdl2_mixer INTERFACE "-s USE_SDL_MIXER=2")
    return()
endif()

find_library(SDL2_MIXER_LIBRARY "SDL2_mixer")
find_path(SDL2_MIXER_INCLUDE_DIR "SDL2/SDL_mixer.h")

mark_as_advanced(SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR)
find_package_handle_standard_args(SDL2_mixer DEFAULT_MSG
  SDL2_MIXER_LIBRARY
  SDL2_MIXER_INCLUDE_DIR
)

if(SDL2_MIXER_FOUND)
  add_library(SDL2::Mixer UNKNOWN IMPORTED)
  set_target_properties(SDL2::Mixer PROPERTIES IMPORTED_LOCATION "${SDL2_MIXER_LIBRARY}")
  target_include_directories(SDL2::Mixer INTERFACE "${SDL2_MIXER_INCLUDE_DIR}/SDL2")
  if(NOT TARGET SDL2::SDL2)
    # otherwise CMake reports an error on the end targets, which is confusing
    message(FATAL_ERROR "Could NOT find target SDL2::SDL2")
  endif()
  target_link_libraries(SDL2::Mixer INTERFACE SDL2::SDL2)
endif()