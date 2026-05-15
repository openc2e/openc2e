# SDL_native_midi

This is the code to play MIDI files through various system-level APIs,
extracted from SDL2_mixer and cleaned up a little.

This is not heavily-maintained, and is barely tested, but we wanted to make it
available for anyone that might be relying on it, as it has been removed from
SDL3_mixer (which still plays MIDI files, but doesn't use OS-provided
facilities, since the OS plays outside our mixer).

This will, in theory, play MIDI files with whatever your platform offers,
including maybe to actual MIDI hardware attached to the machine.

Currently supported platforms:

- Windows
- macOS
- Linux (via ALSA)
- Haiku

There is also a "dummy" platform that fails to init, but can be used on other
platforms so you can still link your program.

It's safe to compile all the files on all platforms. If you compile the macOS
code on Windows, the preprocessor will remove the entire source file, etc.
There is nothing to configure, just compile all the files and link against
SDL3. You don't have to use the included CMake file.

This code likely has bugs, possibly obvious ones; please don't hesitate to
file a bug report and we'll help, but we aren't actively using this code to
find them ourselves.

## Linux

Linux builds will need access to the ALSA headers (On Ubuntu, you can
run `sudo apt-get install libasound-dev`). They do not need to link to ALSA
directly.

## macOS

macOS builds will need to link against the AudioToolbox, AudioUnit, and
CoreServices frameworks.

## Windows

Windows builds will need to link against the system winmm library.


