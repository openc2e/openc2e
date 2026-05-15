# YOU PROBABLY DON'T HAVE TO USE THIS CMAKE PROJECT, IT'S JUST A FEW SOURCE
#  FILES WITH NO CONFIGURATION. COPY IT INTO YOUR PROJECT INSTEAD.

cmake_minimum_required(VERSION 3.12.0)
project(SDL_native_midi)

if(NOT TARGET SDL3::SDL3)
    find_package(SDL3 REQUIRED)
endif()

add_library(SDL_native_midi
    src/SDL_native_midi_common.c
    src/SDL_native_midi_alsa.c
    src/SDL_native_midi_win32.c
    src/SDL_native_midi_dummy.c
    src/SDL_native_midi_macos.c
    # Don't add C++ files if not forced to.
    $<$<PLATFORM_ID:Haiku>:src/SDL_native_midi_haiku.cpp>
)
target_include_directories(SDL_native_midi
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        SDL3::SDL3
)
target_link_libraries(SDL_native_midi
    PRIVATE
        SDL3::SDL3
        $<$<PLATFORM_ID:Windows>:winmm>
        $<$<PLATFORM_ID:Darwin>:-Wl,-framework,AudioToolbox -Wl,-framework,AudioUnit -Wl,-framework,CoreServices>

)
set_property(TARGET SDL_native_midi PROPERTY DEFINE_SYMBOL "DLL_EXPORT")