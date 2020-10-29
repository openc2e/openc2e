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

if(APPLE)
  target_sources(SDL2_mixer PRIVATE
    native_midi/native_midi_common.c
    native_midi/native_midi_macosx.c
  )
  target_link_libraries(SDL2_mixer PRIVATE "-framework AudioToolbox" "-framework CoreFoundation")
  target_compile_definitions(SDL2_mixer PRIVATE MUSIC_MID_NATIVE)
elseif(WIN32)
  target_sources(SDL2_mixer PRIVATE native_midi/native_midi_common.c native_midi/native_midi_win32.c)
  target_link_libraries(SDL2_mixer PRIVATE "winmm")
  target_compile_definitions(SDL2_mixer PRIVATE MUSIC_MID_NATIVE)
else()
  target_sources(SDL2_mixer PRIVATE
    timidity/common.c
    timidity/instrum.c
    timidity/mix.c
    timidity/output.c
    timidity/playmidi.c
    timidity/readmidi.c
    timidity/resample.c
    timidity/tables.c
    timidity/timidity.c
  )
  target_compile_definitions(SDL2_mixer PRIVATE MUSIC_MID_TIMIDITY)
endif()