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

#ifdef SDL_PLATFORM_HAIKU

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MidiStore.h>
#include <MidiDefs.h>
#include <MidiSynthFile.h>
#include <algorithm>

static bool compareMIDIEvent(const MIDIEvent &a, const MIDIEvent &b)
{
    return a.time < b.time;
}

class MidiEventsStore : public BMidi
{
public:
    MidiEventsStore()
    {
        fPlaying = false;
        fLoops = 0;
    }

    virtual status_t Import(SDL_IOStream *src)
    {
        fEvs = NativeMidi_CreateMIDIEventList(src, &fDivision);
        if (!fEvs) {
            return B_BAD_MIDI_DATA;
        }
        fTotal = 0;
        for (MIDIEvent *x = fEvs; x; x = x->next) {
            fTotal++;
        }
        fPos = fTotal;

        sort_events();
        return B_OK;
    }

    virtual void Run()
    {
        fPlaying = true;
        fPos = 0;
        MIDIEvent *ev = fEvs;

        const uint32 startTime = B_NOW;
        while (KeepRunning()) {
            if (!ev) {
                if (fLoops && fEvs) {
                    if (fLoops > 0) {
                        --fLoops;
                    }
                    fPos = 0;
                    ev = fEvs;
                } else {
                    break;
                }
            }
            SprayEvent(ev, ev->time + startTime);
            ev = ev->next;
            fPos++;
        }
        fPos = fTotal;
        fPlaying = false;
    }

    virtual ~MidiEventsStore()
    {
        if (fEvs) {
            NativeMidi_FreeMIDIEventList(fEvs);
            fEvs = 0;
        }
    }

    bool IsPlaying()
    {
        return fPlaying;
    }

    void SetLoops(int loops)
    {
        fLoops = loops;
    }

protected:
    MIDIEvent *fEvs;
    Uint16 fDivision;

    int fPos, fTotal;
    int fLoops;
    bool fPlaying;

    void SprayEvent(MIDIEvent *ev, uint32 time)
    {
        switch (ev->status & 0xF0) {
            case B_NOTE_OFF:
                SprayNoteOff((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
                break;
            case B_NOTE_ON:
                SprayNoteOn((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
                break;
            case B_KEY_PRESSURE:
                SprayKeyPressure((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
                break;
            case B_CONTROL_CHANGE:
                SprayControlChange((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
                break;
            case B_PROGRAM_CHANGE:
                SprayProgramChange((ev->status & 0x0F) + 1, ev->data[0], time);
                break;
            case B_CHANNEL_PRESSURE:
                SprayChannelPressure((ev->status & 0x0F) + 1, ev->data[0], time);
                break;
            case B_PITCH_BEND:
                SprayPitchBend((ev->status & 0x0F) + 1, ev->data[0], ev->data[1], time);
                break;

            case 0xF:
                switch (ev->status) {
                    case B_SYS_EX_START:
                        SpraySystemExclusive(ev->extraData, ev->extraLen, time);
                        break;
                    case B_MIDI_TIME_CODE:
                    case B_SONG_POSITION:
                    case B_SONG_SELECT:
                    case B_CABLE_MESSAGE:
                    case B_TUNE_REQUEST:
                    case B_SYS_EX_END:
                        SpraySystemCommon(ev->status, ev->data[0], ev->data[1], time);
                        break;
                    case B_TIMING_CLOCK:
                    case B_START:
                    case B_STOP:
                    case B_CONTINUE:
                    case B_ACTIVE_SENSING:
                        SpraySystemRealTime(ev->status, time);
                        break;
                    case B_SYSTEM_RESET:
                        if (ev->data[0] == 0x51 && ev->data[1] == 0x03) {
                            SDL_assert(ev->extraLen == 3);
                            const int val = (ev->extraData[0] << 16) | (ev->extraData[1] << 8) | ev->extraData[2];
                            const int tempo = 60000000 / val;
                            SprayTempoChange(tempo, time);
                        } else {
                            SpraySystemRealTime(ev->status, time);
                        }
                        break;
                }
                break;
        }
    }

    void sort_events()
    {
        MIDIEvent *items = new MIDIEvent[fTotal];
        MIDIEvent *x = fEvs;
        for (int i = 0; i < fTotal; i++) {
            SDL_copyp(items + i, x);
            x = x->next;
        }
        std::sort(items, items + fTotal, compareMIDIEvent);

        x = fEvs;

        for (int i = 0; i < fTotal; i++) {
            MIDIEvent *ne = x->next;
            SDL_copyp(x, items + i);
            x->next = ne;
            x = ne;
        }

        for (x = fEvs; x && x->next; x = x->next) {
            SDL_assert(x->time <= x->next->time);
        }
        delete[] items;
    }
};

static BMidiSynth synth;

struct NativeMidi_Song
{
    NativeMidi_MidiEventsStore *store;
};

static NativeMidi_Song *currentSong = NULL;

bool NativeMidi_Init(void)
{
    return (synth.EnableInput(true, false) == B_OK);
}

void NativeMidi_Quit(void)
{
}

void NativeMidi_SetVolume(float volume)
{
    synth.SetVolume(SDL_clamp(volume, 0.0f, 1.0f));
}

NativeMidi_Song *NativeMidi_LoadSong_IO(SDL_IOStream *src, bool closeio)
{
    NativeMidi_Song *song = new NativeMidi_Song;
    song->store = new MidiEventsStore;
    const status_t res = song->store->Import(src);

    if (closeio) {
        SDL_CloseIO(src);
    }

    if (res != B_OK) {
        SDL_SetError("Cannot Import() midi file: status_t=%d", res);
        delete song->store;
        delete song;
        song = NULL;
    }

    return song;
}

void NativeMidi_DestroySong(NativeMidi_Song *song)
{
    if (song != NULL) {
        song->store->Stop();
        song->store->Disconnect(&synth);
        if (currentSong == song) {
            currentSong = NULL;
        }
        delete song->store;
        delete song;
    }
}

void NativeMidi_Start(NativeMidi_Song *song, int loops)
{
    NativeMidi_Stop();
    song->store->Connect(&synth);
    song->store->SetLoops(loops);
    song->store->Start();
    currentSong = song;
}

void NativeMidi_Pause(void)
{
    // !!! FIXME: NativeMidi_Pause is currently unimplemented on Haiku
}

void NativeMidi_Resume(void)
{
    // !!! FIXME: NativeMidi_Resume is currently unimplemented on Haiku
}

void NativeMidi_Stop(void)
{
    if (currentSong != NULL) {
        currentSong->store->Stop();
        currentSong->store->Disconnect(&synth);
        while (currentSong->store->IsPlaying()) {
            SDL_Delay(1);
        }
        currentSong = NULL;
    }
}

bool NativeMidi_Active(void)
{
    return currentSong ? currentSong->store->IsPlaying() : false;
}

#endif  // SDL_PLATFORM_HAIKU
