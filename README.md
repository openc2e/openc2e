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
  * SDL2_mixer
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
  libsdl2-mixer-dev \
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

### Choosing a specific engine to emulate

You can tell openc2e to run as a specific game type with the `-g` / `--gametype` flag. The following values are supported:

| Flag Value | Games Covered                                                 |
|------------|---------------------------------------------------------------|
| `c1`       | Creatures 1                                                   |
| `c2`       | Creatures 2                                                   |
| `c3`       | Creatures 3, Docking Station                                  |
| `cv`       | Creatures Adventures, Creatures Playground, Creatures Village |
| `sm`       | Sea Monkeys                                                   |

This may be useful if you are building a minimal testing environment for a specific game engine.

For example, if you wanted to run as the C2 engine:

```bash
./build/openc2e -g c2 -d /path/to/minimaldata
```

