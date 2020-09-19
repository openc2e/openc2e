cmake_minimum_required(VERSION 2.8.11)
project(SDL_mixer C)

include_directories(include)

add_library(SDL2_mixer
        effect_position.c effects_internal.c effect_stereoreverse.c
        mixer.c music.c
        load_aiff.c load_voc.c
        music_cmd.c music_flac.c
        music_fluidsynth.c music_mad.c
        music_mikmod.c music_modplug.c
        music_mpg123.c music_nativemidi.c
        music_ogg.c music_opus.c
        music_timidity.c music_wav.c)
target_compile_definitions(SDL2_mixer PRIVATE MUSIC_WAV)
target_include_directories(SDL2_mixer PUBLIC .)
target_link_libraries(SDL2_mixer PUBLIC SDL2::SDL2)
