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

#include "SDL_native_midi_common.h"

#if defined(SDL_NATIVE_MIDI_FORCE_DUMMY) || (!defined(SDL_PLATFORM_LINUX) && !defined(SDL_PLATFORM_MACOS) && !defined(SDL_PLATFORM_WIN32) && !defined(SDL_PLATFORM_HAIKU))

bool NativeMidi_Init(void)
{
    SDL_Unsupported();
    return false;
}

void NativeMidi_Quit(void)
{
}

NativeMidi_Song *NativeMidi_LoadSong_IO(SDL_IOStream *src, bool closeio)
{
    if (closeio) {
        SDL_CloseIO(src);
    }
    SDL_Unsupported();
    return NULL;
}

void NativeMidi_DestroySong(NativeMidi_Song *song)
{
}

void NativeMidi_Start(NativeMidi_Song *song, int loops)
{
}

void NativeMidi_Pause(void)
{
}

void NativeMidi_Resume(void)
{
}

void NativeMidi_Stop(void)
{
}

bool NativeMidi_Active(void)
{
}

void NativeMidi_SetVolume(float volume)
{
}

#endif  /* platform check. */

