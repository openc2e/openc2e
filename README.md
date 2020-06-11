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

There are lots of [open issues](https://github.com/openc2e/openc2e/issues) that still need solving.

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

#### Ubuntu

On Ubuntu 18.10 and up, you'll need these packages:

```bash
sudo apt-get install \
  build-essential \
  qtbase5-dev \
  libsdl2-dev \
  libsdl2-gfx-dev \
  libsdl2-mixer-dev \
  libsdl2-net-dev \
  libsdl2-ttf-dev \
  re2c\
  cmake\
  bison\
  libpng-dev
```

### macOS

Install dependencies using [Homebrew](https://brew.sh):

```bash
brew install cmake qt
```

### Windows

Install [Qt](https://www.qt.io/download), [Python](https://www.microsoft.com/en-us/p/python-38/9mssztt1n39l#activetab=pivot:overviewtab), and Visual Studio's [C++ CMake Tools for Windows](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=vs-2019#installation).

Open the folder in Visual Studio and it will automatically run CMake and set up the build system.

## Compiling

Create a new build directory and compile:

```bash
cmake -B build .
make -C build openc2e -j4
```

## Running

At minimum, you should provide a path to a game's data files with the `-d` or `--data-path` flag:

```bash
./build/openc2e -d /path/to/Creatures2
```

The engine will guess the game based on files in the directory, defaulting to C3/DS.

You can tell it to run as a specific game type with the `-g` / `--gametype` flag. The following values are supported:

| Flag Value | Games Covered                                                 |
|------------|---------------------------------------------------------------|
| `c1`       | Creatures 1                                                   |
| `c2`       | Creatures 2                                                   |
| `c3`       | Creatures 3, Docking Station                                  |
| `cv`       | Creatures Adventures, Creatures Playground, Creatures Village |
| `sm`       | Sea Monkeys                                                   |

For example, if you wanted to run Docking Station, you could use:

```bash
./build/openc2e -g c3 -d /path/to/dockingstationdata
```
Specifying a game type may be helpful if you're using:
- a non-standard installation
- data files from a Mac, Linux, or Windows 3.1 version of a game.

For more options, use `-h` / `--help`:
```bash
./build/openc2e --help
```
