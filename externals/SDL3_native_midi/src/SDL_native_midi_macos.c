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

#ifdef SDL_PLATFORM_MACOS

// Mac OS X 10.6+, using Core MIDI.

#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>

// Native Midi song
struct NativeMidi_Song
{
    MusicPlayer player;
    MusicSequence sequence;
    MusicTimeStamp endTime;
    AudioUnit audiounit;
    int loops;
};

static NativeMidi_Song *currentsong = NULL;
static float latched_volume = 1.0f;

static OSStatus GetSequenceLength(MusicSequence sequence, MusicTimeStamp *_sequenceLength)
{
    // https://lists.apple.com/archives/Coreaudio-api/2003/Jul/msg00370.html
    // figure out sequence length
    UInt32 ntracks, i;
    MusicTimeStamp sequenceLength = 0;
    OSStatus err;

    err = MusicSequenceGetTrackCount(sequence, &ntracks);
    if (err != noErr) {
        return err;
    }

    for (i = 0; i < ntracks; ++i)
    {
        MusicTrack track;
        MusicTimeStamp tracklen = 0;
        UInt32 tracklenlen = sizeof (tracklen);

        err = MusicSequenceGetIndTrack(sequence, i, &track);
        if (err != noErr) {
            return err;
        }

        err = MusicTrackGetProperty(track, kSequenceTrackProperty_TrackLength, &tracklen, &tracklenlen);
        if (err != noErr) {
            return err;
        }

        if (sequenceLength < tracklen) {
            sequenceLength = tracklen;
        }
    }

    *_sequenceLength = sequenceLength;

    return noErr;
}

static OSStatus GetSequenceAudioUnitMatching(MusicSequence sequence, AudioUnit *aunit, OSType type, OSType subtype)
{
    AUGraph graph;
    UInt32 nodecount, i;
    OSStatus err;

    err = MusicSequenceGetAUGraph(sequence, &graph);
    if (err != noErr) {
        return err;
    }

    err = AUGraphGetNodeCount(graph, &nodecount);
    if (err != noErr) {
        return err;
    }

    for (i = 0; i < nodecount; i++) {
        AUNode node;
        AudioComponentDescription desc;

        if (AUGraphGetIndNode(graph, i, &node) != noErr) {
            continue;  // better luck next time.
        } else if (AUGraphNodeInfo(graph, node, &desc, aunit) != noErr) {
            continue;
        } else if (desc.componentType != type) {
            continue;
        } else if (desc.componentSubType != subtype) {
            continue;
        }
        return noErr;  // found it!
    }

    *aunit = NULL;
    return kAUGraphErr_NodeNotFound;
}

#if 0  // !!! FIXME: needs the soundfont APIs from SDL2_Mixer. Is this necessary...?
typedef struct
{
    AudioUnit aunit;
    bool soundfont_set;
    CFURLRef default_url;
} macosx_load_soundfont_ctx;

static bool SDLCALL macosx_load_soundfont(const char *path, void *data)
{
    macosx_load_soundfont_ctx *ctx = (macosx_load_soundfont_ctx *) data;
    if (ctx->soundfont_set) {
        return false;
    }

    CFURLRef url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8*)path, strlen(path), false);
    if (!url) {
        return false;
    }

    OSStatus err = AudioUnitSetProperty(ctx->aunit, kMusicDeviceProperty_SoundBankURL,
                               kAudioUnitScope_Global, 0, &url, sizeof(url));
    CFRelease(url);
    if (err != noErr) {
        if (ctx->default_url) {
            err = AudioUnitSetProperty(ctx->aunit,
                                       kMusicDeviceProperty_SoundBankURL,
                                       kAudioUnitScope_Global, 0,
                                       &ctx->default_url, sizeof(CFURLRef));
            if (err != noErr) {
                // uh-oh, this might leave the audio unit in an unusable state
                //   (e.g. if the soundfont was an incompatible file type)
            }
        }
        return false;
    }

    ctx->soundfont_set = true;
    return true;
}

static void SetSequenceSoundFont(MusicSequence sequence)
{
    macosx_load_soundfont_ctx ctx;
    ctx.soundfont_set = false;
    ctx.default_url = NULL;

    CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.audio.units.Components"));
    if (bundle) {
        ctx.default_url = CFBundleCopyResourceURL(bundle, CFSTR("gs_instruments"), CFSTR("dls"), NULL);
    }

    OSStatus err = GetSequenceAudioUnitMatching(sequence, &ctx.aunit, kAudioUnitType_MusicDevice, kAudioUnitSubType_DLSSynth);
    if (err != noErr) {
        return;
    }

    Mix_EachSoundFont(macosx_load_soundfont, &ctx);
    if (ctx.default_url) {
        CFRelease(ctx.default_url);
    }
}
#endif

bool NativeMidi_Init(void)
{
    return true;  // always available.
}

void NativeMidi_Quit(void)
{
}

NativeMidi_Song *NativeMidi_LoadSong_IO(SDL_IOStream *src, bool closeio)
{
    NativeMidi_Song *retval = NULL;
    void *buf = NULL;
    size_t len = 0;
    CFDataRef data = NULL;

    buf = SDL_LoadFile_IO(src, &len, false);
    if (buf == NULL) {
        goto fail;
    }

    retval = SDL_calloc(1, sizeof(NativeMidi_Song));
    if (retval == NULL) {
        goto fail;
    } else if (NewMusicPlayer(&retval->player) != noErr) {
        goto fail;
    } else if (NewMusicSequence(&retval->sequence) != noErr) {
        goto fail;
    }

    data = CFDataCreate(NULL, (const UInt8 *) buf, len);
    if (data == NULL) {
        goto fail;
    }

    SDL_free(buf);
    buf = NULL;

    if (MusicSequenceFileLoadData(retval->sequence, data, 0, 0) != noErr) {
        goto fail;
    }

    CFRelease(data);
    data = NULL;

    if (GetSequenceLength(retval->sequence, &retval->endTime) != noErr) {
        goto fail;
    } else if (MusicPlayerSetSequence(retval->player, retval->sequence) != noErr) {
        goto fail;
    } else if (closeio) {
        SDL_CloseIO(src);
    }

    return retval;

fail:
    if (retval) {
        if (retval->sequence) {
            DisposeMusicSequence(retval->sequence);
        }
        if (retval->player) {
            DisposeMusicPlayer(retval->player);
        }
        SDL_free(retval);
    }

    if (data) {
        CFRelease(data);
    }

    if (buf) {
        SDL_free(buf);
    }

    return NULL;
}

void NativeMidi_DestroySong(NativeMidi_Song *song)
{
    if (song != NULL) {
        if (currentsong == song) {
            currentsong = NULL;
        }
        MusicPlayerStop(song->player);

        // needed to prevent error and memory leak when disposing sequence
        MusicPlayerSetSequence(song->player, NULL);

        DisposeMusicSequence(song->sequence);
        DisposeMusicPlayer(song->player);
        SDL_free(song);
    }
}

static NativeMidi_Song* paused_song = NULL;
static MusicTimeStamp paused_time = 0;
static bool resume = false;

void NativeMidi_Start(NativeMidi_Song *song, int loops)
{
    if (!resume) {
        // If we are not resuming a paused song, clear any existing paused info.
        paused_song = NULL;
        paused_time = 0;
    }

    if (song) {
        if (currentsong) {
            MusicPlayerStop(currentsong->player);
        }

        currentsong = song;
        currentsong->loops = loops;

        MusicPlayerPreroll(song->player);
        GetSequenceAudioUnitMatching(song->sequence, &song->audiounit, kAudioUnitType_Output, kAudioUnitSubType_DefaultOutput);
        #if 0  // !!! FIXME: needs the soundfont APIs from SDL2_Mixer. Is this necessary...?
        SetSequenceSoundFont(song->sequence);
        #endif

        const float vol = latched_volume;
        latched_volume += 1.0f;  // +1 just make this not match.
        NativeMidi_SetVolume(vol);

        MusicPlayerSetTime(song->player, resume ? paused_time : 0);
        MusicPlayerStart(song->player);
    }
}

void NativeMidi_Pause(void)
{
    if (currentsong) {
        paused_song = currentsong;
        MusicPlayerGetTime(currentsong->player, &paused_time);
        NativeMidi_Stop();
    }
}

void NativeMidi_Resume(void)
{
    if (paused_song) {
        resume = true;
        NativeMidi_Start(paused_song, paused_song->loops);
        paused_song = NULL;
        paused_time = 0;
        resume = false;
    }
}

void NativeMidi_Stop(void)
{
    if (currentsong) {
        MusicPlayerStop(currentsong->player);
        currentsong = NULL;
    }
}

bool NativeMidi_Active(void)
{
    MusicTimeStamp currentTime = 0;
    NativeMidi_Song* song = currentsong ? currentsong : paused_song;

    if (song == NULL) {
        return false;
    }
    
    if (paused_song) {
        currentTime = paused_time;
    } else {
        MusicPlayerGetTime(song->player, &currentTime);
    }

    if ((currentTime < song->endTime) || (currentTime >= kMusicTimeStamp_EndOfTrack)) {
        return true;
    } else if (song->loops) {
        --song->loops;
        if (paused_song) {
            paused_time = 0;
        } else {
            MusicPlayerSetTime(song->player, 0);
        }
        return true;
    }
    return false;
}

void NativeMidi_SetVolume(float volume)
{
    if (latched_volume != volume) {
        latched_volume = SDL_clamp(volume, 0.0f, 1.0f);
        if (currentsong && currentsong->audiounit) {
            AudioUnitSetParameter(currentsong->audiounit, kHALOutputParam_Volume, kAudioUnitScope_Global, 0, latched_volume, 0);
        }
    }
}

#endif

