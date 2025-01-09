![openc2e logo](https://raw.githubusercontent.com/ligfx/openc2e/master/Openc2e-logo2008.png)

# openc2e

openc2e is a free and open-source game engine for the [_Creatures_](https://creatures.wiki) artificial life games.

The goal is to allow you to play games such as Creatures, Creatures 2, Creatures 3, Docking Station and more, on many different platforms.

Want to know more? Come visit us on the [Caos Coding Cave Discord](https://discord.gg/rWFC3b3).

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

There are lots of [open issues](https://github.com/openc2e/openc2e/issues) that still need solving.

# Building

## Dependencies

openc2e depends on:

* SDL2
  * SDL2_mixer
* Python
* CMake
* Boost.Serialization (optional)

### Linux

#### Ubuntu

On Ubuntu 18.10 and up, you'll need these packages:

```bash
sudo apt-get install \
  build-essential \
  libsdl2-dev \
  libsdl2-mixer-dev \
  cmake
```

### macOS

Install dependencies using [Homebrew](https://brew.sh):

```bash
brew install cmake
```

### Windows

Install [Python](https://www.microsoft.com/en-us/p/python-38/9mssztt1n39l#activetab=pivot:overviewtab), and Visual Studio's [C++ CMake Tools for Windows](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2019#installation).

Open the folder in Visual Studio and it will automatically run CMake and set up the build system.

## Compiling

Create a new build directory and compile:

```bash
cmake -B build .
make -C build openc2e -j4
```

## Running

You should provide a path to a game's data files with the `-d` or `--data-path` flag:

```bash
./build/openc2e -d /path/to/Creatures2
```

The engine will try to guess the game based on files in the directory. If it can't decide, it will default to C3/DS.

## Advanced Usage

To see an overview of all available options, use `-h` / `--help`:
```bash
./build/openc2e --help
```
