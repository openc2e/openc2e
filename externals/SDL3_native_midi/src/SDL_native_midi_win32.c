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

// everything below is currently one very big bad hack ;) Proff

#ifdef SDL_PLATFORM_WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

struct NativeMidi_Song
{
    bool MusicLoaded;
    bool MusicPlaying;
    int Loops;
    int CurrentHdr;
    MIDIHDR MidiStreamHdr[2];
    MIDIEVENT *NewEvents;
    Uint16 ppqn;
    int Size;
    int NewPos;
    SDL_Mutex *mutex;
};

static UINT MidiDevice = MIDI_MAPPER;
static HMIDISTRM hMidiStream;
static NativeMidi_Song *currentsong;

static bool BlockOut(NativeMidi_Song *song)
{
    int BlockSize;
    MMRESULT err;
    MIDIHDR *hdr;

    if ((song->MusicLoaded) && (song->NewEvents)) {
        // proff 12/8/98: Added for safety
        song->CurrentHdr = !song->CurrentHdr;
        hdr = &song->MidiStreamHdr[song->CurrentHdr];
        midiOutUnprepareHeader((HMIDIOUT)hMidiStream, hdr, sizeof(MIDIHDR));
        if (song->NewPos >= song->Size) {
            return false;
        }

        BlockSize = (song->Size-song->NewPos);
        if (BlockSize <= 0) {
            return false;
        }

        if (BlockSize > 36000) {
            BlockSize = 36000;
        }

        hdr->lpData = (void *)((unsigned char *)song->NewEvents + song->NewPos);
        song->NewPos += BlockSize;
        hdr->dwBufferLength = BlockSize;
        hdr->dwBytesRecorded = BlockSize;
        hdr->dwFlags = 0;
        hdr->dwOffset = 0;

        err = midiOutPrepareHeader((HMIDIOUT)hMidiStream, hdr, sizeof(MIDIHDR));
        if (err != MMSYSERR_NOERROR) {
            return false;
        }

        err = midiStreamOut(hMidiStream, hdr, sizeof(MIDIHDR));
        return false;
    }

    return true;
}

static void MIDItoStream(NativeMidi_Song *song, MIDIEvent *evntlist)
{
    MIDIEvent *event = evntlist;
    MIDIEVENT *newevent;
    int eventcount = 0;
    int time = 0;

    while (event) {
        eventcount++;
        event = event->next;
    }

    song->NewEvents = SDL_calloc(eventcount, 3 * sizeof(DWORD));
    if (!song->NewEvents) {
        return;
    }

    eventcount = 0;
    event = evntlist;
    newevent = song->NewEvents;
    while (event) {
        const int status = (event->status & 0xF0) >> 4;
        switch (status) {
            case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_NOTE_ON:
            case MIDI_STATUS_AFTERTOUCH:
            case MIDI_STATUS_CONTROLLER:
            case MIDI_STATUS_PROG_CHANGE:
            case MIDI_STATUS_PRESSURE:
            case MIDI_STATUS_PITCH_WHEEL:
                newevent->dwDeltaTime = event->time;
                newevent->dwEvent = (event->status|0x80) | (event->data[0] << 8) | (event->data[1] << 16) | (MEVT_SHORTMSG << 24);
                newevent=(MIDIEVENT*)((char*)newevent + (3 * sizeof(DWORD)));
                eventcount++;
                break;

            case MIDI_STATUS_SYSEX:
                if ((event->status == 0xFF) && (event->data[0] == 0x51)) {  // Tempo change
                    const int tempo = (event->extraData[0] << 16) | (event->extraData[1] << 8) | event->extraData[2];
                    newevent->dwDeltaTime = event->time;
                    newevent->dwEvent = (MEVT_TEMPO<<24) | tempo;
                    newevent=(MIDIEVENT*)((char*)newevent + (3 * sizeof(DWORD)));
                    eventcount++;
                }
                break;
        }

        event = event->next;
    }

    song->Size = eventcount * 3 * sizeof(DWORD);

    song->NewPos = 0;
    newevent = song->NewEvents;
    while (song->NewPos<song->Size) {
        const int temptime = newevent->dwDeltaTime;
        newevent->dwDeltaTime -= time;
        time = temptime;
        if ((song->NewPos + 12) >= song->Size) {
            newevent->dwEvent |= MEVT_F_CALLBACK;
        }
        newevent = (MIDIEVENT*)((char*)newevent + (3 * sizeof(DWORD)));
        song->NewPos += 12;
    }
    song->NewPos = 0;
    song->MusicLoaded = true;
}

void CALLBACK MidiProc( HMIDIIN hMidi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2 )
{
    NativeMidi_Song *song = (NativeMidi_Song *)dwInstance;
    (void)hMidi;
    (void)dwParam2;

    if (!song) {
        return;
    }

    SDL_LockMutex(song->mutex);

    switch (uMsg) {
        case MOM_DONE:
            if (song->MusicPlaying && song->MusicLoaded && (dwParam1 == (DWORD_PTR)&song->MidiStreamHdr[song->CurrentHdr])) {
                BlockOut(song);
            }
            break;

        case MOM_POSITIONCB:
            if (song->MusicPlaying && song->MusicLoaded && (dwParam1 == (DWORD_PTR)&song->MidiStreamHdr[song->CurrentHdr])) {
                if (song->Loops) {
                    if (song->Loops > 0) {
                        --song->Loops;
                    }
                    song->NewPos=0;
                    BlockOut(song);
                } else {
                    song->MusicPlaying = false;
                }
            }
            break;

        case MOM_CLOSE:
            song->MusicPlaying = false;
            break;

        default:
            break;
    }

    SDL_UnlockMutex(song->mutex);
}

bool NativeMidi_Init(void)
{
    HMIDISTRM MidiStream;
    const MMRESULT merr = midiStreamOpen(&MidiStream,&MidiDevice,(DWORD)1,(DWORD_PTR)MidiProc,(DWORD_PTR)0,CALLBACK_FUNCTION);

    if (merr != MMSYSERR_NOERROR) {
        return false;
    }

    midiStreamClose(MidiStream);
    return true;
}

void NativeMidi_Quit(void)
{
}

NativeMidi_Song *NativeMidi_LoadSong_IO(SDL_IOStream *src, bool closeio)
{
    NativeMidi_Song *newsong = (NativeMidi_Song *) SDL_malloc(sizeof(NativeMidi_Song));
    if (!newsong) {
        return NULL;
    }
    SDL_zerop(newsong);

    newsong->mutex = SDL_CreateMutex();
    if (!newsong->mutex) {
        SDL_free(newsong);
        return NULL;
    }

    // Attempt to load the midi file
    MIDIEvent *evntlist = NativeMidi_CreateMIDIEventList(src, &newsong->ppqn);
    if (!evntlist) {
        SDL_DestroyMutex(newsong->mutex);
        SDL_free(newsong);
        return NULL;
    }

    MIDItoStream(newsong, evntlist);

    NativeMidi_FreeMIDIEventList(evntlist);

    if (closeio) {
        SDL_CloseIO(src);
    }
    return newsong;
}

void NativeMidi_DestroySong(NativeMidi_Song *song)
{
    if (song) {
        SDL_free(song->NewEvents);
        SDL_DestroyMutex(song->mutex);
        SDL_free(song);
    }
}

void NativeMidi_Start(NativeMidi_Song *song, int loops)
{
    NativeMidi_Stop();
    if (!hMidiStream) {
        MIDIPROPTIMEDIV mptd;
        MMRESULT merr = midiStreamOpen(&hMidiStream,&MidiDevice,(DWORD)1,(DWORD_PTR)MidiProc,(DWORD_PTR)song,CALLBACK_FUNCTION);
        if (merr != MMSYSERR_NOERROR) {
            hMidiStream = NULL; // should I do midiStreamClose(hMidiStream) before?
            return;
        }

        // midiStreamStop(hMidiStream);
        currentsong = song;
        currentsong->NewPos = 0;
        currentsong->MusicPlaying = true;
        currentsong->Loops = loops;
        mptd.cbStruct = sizeof(MIDIPROPTIMEDIV);
        mptd.dwTimeDiv = currentsong->ppqn;
        merr = midiStreamProperty(hMidiStream, (LPBYTE)&mptd, MIDIPROP_SET | MIDIPROP_TIMEDIV);
        BlockOut(song);
        merr = midiStreamRestart(hMidiStream);
    }
}

void NativeMidi_Pause(void)
{
    if (hMidiStream) {
        midiStreamPause(hMidiStream);
    }
}

void NativeMidi_Resume(void)
{
    if (hMidiStream) {
        midiStreamRestart(hMidiStream);
    }
}

void NativeMidi_Stop(void)
{
    NativeMidi_Song *song = currentsong;

    if (hMidiStream) {
        SDL_LockMutex(song->mutex);
        midiStreamStop(hMidiStream);
        midiStreamClose(hMidiStream);
        currentsong=NULL;
        hMidiStream = NULL;
        SDL_UnlockMutex(song->mutex);
    }
}

bool NativeMidi_Active(void)
{
    return hMidiStream && currentsong && currentsong->MusicPlaying;
}

void NativeMidi_SetVolume(float volume)
{
    const int ivolume = (int) (SDL_clamp(volume, 0.0f, 1.0f) * 128.0f);
    const int calcVolume = ((65535 * ivolume) / 128);
    midiOutSetVolume((HMIDIOUT)hMidiStream, MAKELONG(calcVolume , calcVolume));
}

#endif // Windows native MIDI support
