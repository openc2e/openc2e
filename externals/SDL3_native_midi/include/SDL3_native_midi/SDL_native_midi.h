/*
  SDL_native_midi: Platform-specific MIDI support.
  Copyright (C) 2000-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
 * Several other people have listed copyrights in this code over the years,
 * when it was part of SDL_mixer. The original author was Florian Schulze.
 *
 * Here is a list:
 *
 * Copyright (C) 2000,2001  Florian 'Proff' Schulze <florian.proff.schulze@gmx.net>
 * Copyright (C) 2009 Ryan C. Gordon <icculus@icculus.org>
 * Copyright (C) 2010 Egor Suvorov <egor_suvorov@mail.ru>
 * Copyright (C) 2024 Tasos Sahanidis <code@tasossah.com>
 */

#ifndef SDL_NATIVE_MIDI_H_
#define SDL_NATIVE_MIDI_H_

#include <SDL3/SDL.h>
#include <SDL3/SDL_begin_code.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct NativeMidi_Song NativeMidi_Song;

extern SDL_DECLSPEC bool SDLCALL NativeMidi_Init(void);
extern SDL_DECLSPEC void SDLCALL NativeMidi_Quit(void);
extern SDL_DECLSPEC NativeMidi_Song * SDLCALL NativeMidi_LoadSong_IO(SDL_IOStream *src, bool closeio);
extern SDL_DECLSPEC NativeMidi_Song * SDLCALL NativeMidi_LoadSong(const char *path);
extern SDL_DECLSPEC void SDLCALL NativeMidi_DestroySong(NativeMidi_Song *song);
extern SDL_DECLSPEC void SDLCALL NativeMidi_Start(NativeMidi_Song *song, int loops);

/* !!! FIXME: these are not hooked up on Haiku OS! */
/* (Works on ALSA, macOS, and Windows, though!) */
extern SDL_DECLSPEC void SDLCALL NativeMidi_Pause(void);
extern SDL_DECLSPEC void SDLCALL NativeMidi_Resume(void);

extern SDL_DECLSPEC void SDLCALL NativeMidi_Stop(void);
extern SDL_DECLSPEC bool SDLCALL NativeMidi_Active(void);
extern SDL_DECLSPEC void SDLCALL NativeMidi_SetVolume(float volume);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#include <SDL3/SDL_close_code.h>

#endif /* SDL_NATIVE_MIDI_H_ */
