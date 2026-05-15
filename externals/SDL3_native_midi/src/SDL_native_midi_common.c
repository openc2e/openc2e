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

// The constant 'MThd'
#define MIDI_MAGIC	0x4d546864
// The constant 'RIFF'
#define RIFF_MAGIC	0x52494646

// A single midi track as read from the midi file
typedef struct
{
    Uint8 *data;                    // MIDI message stream
    int len;                        // length of the track data
} MIDITrack;

// A midi file, stripped down to the absolute minimum - divison & track data
typedef struct
{
    int division;                   // number of pulses per quarter note (ppqn)
    int nTracks;                    // number of tracks
    MIDITrack *track;               // tracks
} MIDIFile;


// Get Variable Length Quantity
static int GetVLQ(MIDITrack *track, int *currentPos)
{
    int l = 0;
    Uint8 c;
    while (1) {
        c = track->data[*currentPos];
        (*currentPos)++;
        l += (c & 0x7f);
        if (!(c & 0x80)) {
            return l;
        }
        l <<= 7;
    }
}

// Create a single MIDIEvent
static MIDIEvent *CreateMIDIEvent(Uint32 time, Uint8 event, Uint8 a, Uint8 b)
{
    MIDIEvent *newEvent;

    newEvent = SDL_calloc(1, sizeof(MIDIEvent));

    if (newEvent) {
        newEvent->time = time;
        newEvent->status = event;
        newEvent->data[0] = a;
        newEvent->data[1] = b;
    }

    return newEvent;
}

// Convert a single midi track to a list of MIDIEvents
static MIDIEvent *MIDITracktoStream(MIDITrack *track)
{
    Uint32 atime = 0;
    Uint32 len = 0;
    Uint8 event,type,a,b;
    Uint8 laststatus = 0;
    Uint8 lastchan = 0;
    int currentPos = 0;
    int end = 0;
    MIDIEvent *head = CreateMIDIEvent(0,0,0,0); // dummy event to make handling the list easier
    MIDIEvent *currentEvent = head;

    while (!end) {
        if (currentPos >= track->len) {
            break; // End of data stream reached
        }

        atime += GetVLQ(track, &currentPos);
        event = track->data[currentPos++];

        // Handle SysEx seperatly
        if (((event>>4) & 0x0F) == MIDI_STATUS_SYSEX) {
            if (event == 0xFF) {
                type = track->data[currentPos];
                currentPos++;
                switch(type) {
                    case 0x2f: // End of data marker
                        end = 1;
                    case 0x51: // Tempo change
                        /*
                        a=track->data[currentPos];
                        b=track->data[currentPos+1];
                        c=track->data[currentPos+2];
                        AddEvent(song, atime, MEVT_TEMPO, c, b, a);
                        */
                        break;
                }
            } else {
                type = 0;
            }

            len = GetVLQ(track, &currentPos);

            // Create an event and attach the extra data, if any
            currentEvent->next = CreateMIDIEvent(atime, event, type, 0);
            currentEvent = currentEvent->next;
            if (NULL == currentEvent) {
                NativeMidi_FreeMIDIEventList(head);
                return NULL;
            }
            if (len) {
                currentEvent->extraLen = len;
                currentEvent->extraData = SDL_malloc(len);
                SDL_memcpy(currentEvent->extraData, &(track->data[currentPos]), len);
                currentPos += len;
            }
        } else {
            a = event;
            if (a & 0x80) { // It's a status byte
                // Extract channel and status information
                lastchan = a & 0x0F;
                laststatus = (a>>4) & 0x0F;

                // Read the next byte which should always be a data byte
                a = track->data[currentPos++] & 0x7F;
            }
            switch(laststatus) {
                case MIDI_STATUS_NOTE_OFF:
                case MIDI_STATUS_NOTE_ON: // Note on
                case MIDI_STATUS_AFTERTOUCH: // Key Pressure
                case MIDI_STATUS_CONTROLLER: // Control change
                case MIDI_STATUS_PITCH_WHEEL: // Pitch wheel
                    b = track->data[currentPos++] & 0x7F;
                    currentEvent->next = CreateMIDIEvent(atime, (Uint8)((laststatus<<4)+lastchan), a, b);
                    currentEvent = currentEvent->next;
                    if (NULL == currentEvent) {
                        NativeMidi_FreeMIDIEventList(head);
                        return NULL;
                    }
                    break;

                case MIDI_STATUS_PROG_CHANGE: // Program change
                case MIDI_STATUS_PRESSURE: // Channel pressure
                    a &= 0x7f;
                    currentEvent->next = CreateMIDIEvent(atime, (Uint8)((laststatus<<4)+lastchan), a, 0);
                    currentEvent = currentEvent->next;
                    if (NULL == currentEvent) {
                        NativeMidi_FreeMIDIEventList(head);
                        return NULL;
                    }
                    break;

                default: // Sysex already handled above
                    break;
            }
        }
    }

    currentEvent = head->next;
    SDL_free(head); // release the dummy head event
    return currentEvent;
}

/*
 *  Convert a midi song, consisting of up to 32 tracks, to a list of MIDIEvents.
 *  To do so, first convert the tracks seperatly, then interweave the resulting
 *  MIDIEvent-Lists to one big list.
 */
static MIDIEvent *MIDItoStream(MIDIFile *mididata)
{
    MIDIEvent **track;
    MIDIEvent *head = CreateMIDIEvent(0,0,0,0); // dummy event to make handling the list easier
    MIDIEvent *currentEvent = head;
    int trackID;

    if (NULL == head) {
        return NULL;
    }

    track = (MIDIEvent**) SDL_calloc(1, sizeof(MIDIEvent*) * mididata->nTracks);
    if (NULL == track) {
        SDL_free(head);
        return NULL;
    }

    // First, convert all tracks to MIDIEvent lists
    for (trackID = 0; trackID < mididata->nTracks; trackID++) {
        track[trackID] = MIDITracktoStream(&mididata->track[trackID]);
    }

    // Now, merge the lists.
    // TODO
    while (1) {
        Uint32 lowestTime = 0x7FFFFFFF; // INT_MAX
        int currentTrackID = -1;

        // Find the next event
        for (trackID = 0; trackID < mididata->nTracks; trackID++) {
            if (track[trackID] && (track[trackID]->time < lowestTime)) {
                currentTrackID = trackID;
                lowestTime = track[currentTrackID]->time;
            }
        }

        // Check if we processes all events
        if (currentTrackID == -1) {
            break;
        }

        currentEvent->next = track[currentTrackID];
        track[currentTrackID] = track[currentTrackID]->next;

        currentEvent = currentEvent->next;

        lowestTime = 0;
    }

    // Make sure the list is properly terminated
    currentEvent->next = NULL;

    currentEvent = head->next;
    SDL_free(track);
    SDL_free(head); // release the dummy head event
    return currentEvent;
}

static int ReadMIDIFile(MIDIFile *mididata, SDL_IOStream *src)
{
    int i = -1;
    Uint32 ID = 0;
    Uint32 size = 0;
    Uint16 format = 0;
    Uint16 tracks = 0;
    Uint16 division = 0;

    if (!mididata) {
        return 0;
    }
    if (!src) {
        return 0;
    }

    // Make sure this is really a MIDI file
    if (!SDL_ReadU32BE(src, &ID)) {
        return 0;
    }
    if (ID == RIFF_MAGIC) {
        SDL_SeekIO(src, 16, SDL_IO_SEEK_CUR);
        if (!SDL_ReadU32BE(src, &ID)) {
            return 0;
        }
    }
    if (ID != MIDI_MAGIC) {
        return 0;
    }

    // Header size must be 6
    if (!SDL_ReadU32BE(src, &size)) {
        return 0;
    }
    if (size != 6) {
        return 0;
    }

    // We only support format 0 and 1, but not 2
    if (!SDL_ReadU16BE(src, &format)) {
        return 0;
    } else if (format != 0 && format != 1) {
        return 0;
    }

    if (!SDL_ReadU16BE(src, &tracks)) {
        return 0;
    }
    mididata->nTracks = tracks;

    // Allocate tracks
    mididata->track = (MIDITrack*) SDL_calloc(1, sizeof(MIDITrack) * mididata->nTracks);
    if (NULL == mididata->track) {
        goto bail;
    }

    // Retrieve the PPQN value, needed for playback
    if (!SDL_ReadU16BE(src, &division)) {
        goto bail;
    }
    mididata->division = division;

    for (i = 0; i < tracks; i++) {
        if (!SDL_ReadU32BE(src, &ID)) {
            goto bail;
        } else if (!SDL_ReadU32BE(src, &size)) {
            goto bail;
        }
        mididata->track[i].len = size;
        mididata->track[i].data = SDL_malloc(size);
        if (!mididata->track[i].data) {
            goto bail;
        }
        if (SDL_ReadIO(src, mididata->track[i].data, size) != size) {
            goto bail;
        }
    }
    return 1;

bail:
    for (; i >= 0; i--) {
        if (mididata->track[i].data) {
            SDL_free(mididata->track[i].data);
        }
    }

    SDL_free(mididata->track);
    return 0;
}

MIDIEvent *NativeMidi_CreateMIDIEventList(SDL_IOStream *src, Uint16 *division)
{
    MIDIFile *mididata = NULL;
    MIDIEvent *eventList;
    int trackID;

    mididata = SDL_calloc(1, sizeof(MIDIFile));
    if (!mididata) {
        return NULL;
    }

    // Open the file
    if (src != NULL) {
        // Read in the data
        if (!ReadMIDIFile(mididata, src)) {
            SDL_free(mididata);
            return NULL;
        }
    } else {
        SDL_free(mididata);
        return NULL;
    }

    if (division) {
        *division = mididata->division;
    }

    eventList = MIDItoStream(mididata);
    if (eventList == NULL) {
        SDL_free(mididata);
        return NULL;
    }
    for (trackID = 0; trackID < mididata->nTracks; trackID++) {
        if (mididata->track[trackID].data) {
            SDL_free(mididata->track[trackID].data);
        }
    }
    SDL_free(mididata->track);
    SDL_free(mididata);

    return eventList;
}

void NativeMidi_FreeMIDIEventList(MIDIEvent *head)
{
    MIDIEvent *cur, *next;

    cur = head;

    while (cur) {
        next = cur->next;
        if (cur->extraData) {
            SDL_free(cur->extraData);
        }
        SDL_free(cur);
        cur = next;
    }
}

NativeMidi_Song *NativeMidi_LoadSong(const char *path)
{
    SDL_IOStream *io = SDL_IOFromFile(path, "rb");
    return io ? NativeMidi_LoadSong_IO(io, true) : NULL;
}
