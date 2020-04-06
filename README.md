![openc2e logo](https://raw.githubusercontent.com/ligfx/openc2e/master/Openc2e-logo2008.png)


# openc2e

openc2e is an open-source game engine intended to run all the games in the
[_Creatures_](https://creatures.wiki) series.

# Games

openc2e is intended to support:

* Creatures (1996)
* Creatures 2 (1998)
* Creatures 3 (1999)
* Docking Station (2001)
* Creatures Playground (1999)
* Creatures Adventures (2000)

# Status

## Working

* Agents (COBs) work
* Imperfect physics
* Creatures biochemistry
* Sound effects

## Todo

* Actual creatures (C3+ creatures work somewhat, C1 and C2 ones don't work at all)
* Physics improvements (Especially C3+)
* Network ability (the Docking Station warp)
* Serialization (world saving, creature exporting, ...)
* Music

There are lots of [open issues on the old Google Code issue tracker](https://code.google.com/archive/p/openc2e/issues) that still need solving.

# Building

## Dependencies

openc2e depends on:

* SDL2
  * SDL2_gfx
  * SDL2_mixer
  * SDL2_net
  * SDL2_ttf
* bison
* re2c
* Python
* Qt5
* CMake
* Boost.Serialization (optional)

### Linux

On Ubuntu 18.10 you'll need these packages:

```bash
sudo apt-get install \
  build-essential \
  qtbase5-dev \
  libsdl2-dev \
  libsdl2-gfx-dev \
  libsdl2-mixer-dev \
  libsdl2-net-dev \
  libsdl2-ttf-dev \
  re2c
```

### macOS

Install dependencies using [Homebrew](https://brew.sh):

```bash
brew install sdl2 sdl2_gfx sdl2_mixer sdl2_net sdl2_ttf re2c cmake qt
```

### Windows

Good luck! Open a PR if you get a good process for building on Windows.

## Compiling

Create a new build directory and compile:

```bash
cmake -B build .
make -C build openc2e -j4
```

## Running

You need to provide openc2e with the path to the game's data, for example:

```bash
./build/openc2e -d /path/to/Creatures2
```
