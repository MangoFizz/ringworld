# ringworld

As with other similar projects, the goal of this project is to study and create a free open-source re-implementation of the Halo Custom Edition's executable.

Ringworld works like any other mod, patching functions in the game executable to redirect them to its reimplementations. This allows us to add new features, fix bugs, and improve the game in general.

This project comes from the codebase of [Demon](https://github.com/Aerocatia/demon), a project with the same goal, but focused on the Halo PC Demo executable.

## Disclaimer

This project is created for educational and research purposes, in an effort to preserve and improve the Halo Custom Edition gameplay experience. It is not intended to promote illegal activities or violate intellectual property laws. This is NOT a replacement for the game, but an extension of it, so you will still need a copy of the original game to play. This repository does not include any executables or assets of the game.

## Features

One of the long-term goals of this project is to implement all the features from other mods, such as Chimera, Open Sauce, and SAPP, and add new ones.

Some of the features we plan to implement are:

- Interpolation (60+ FPS support)
- Multiplayer chat replacement
- Shader transparent generic support
- HUD meters Xbox shader restoration
- Loading screen from the Xbox version
- Extended limits for game state resources
- Quality of life improvements and bug fixes

Currently, the project has the following features:

- Tag data buffer upgraded to 64MiB
- New assertions for debugging

## Installation

At this point, the project is in a very early stage of development, so it can be installed as any other mod. It can be loaded with Chimera's mod loader, which is the recommended way to use it for now. See the [Chimera's repository](https://github.com/SnowyMouse/chimera?tab=readme-ov-file#mod-support) for more information.

## Building

To build the project, you will need to have the following dependencies installed:

- CMake
- Mingw-w64
- Python
- Lua 5.3

### Procedure

1. Clone the repository.
```
git clone https://github.com/MangoFizz/ringworld && cd ringworld
```
2. Create a build folder.
```
mkdir build && cd build
```
3. Generate the build files.
```
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../toolchain-i686-w64-mingw32.cmake
```
4. Build the project.
```
cmake --build .
```

Depending on your system, you may need to modify the toolchain file to match your environment.

## Sources

There are some projects and sites that have been used as a reference for understanding the game and creating this project. Some of them are:

- [Demon](https://github.com/Aerocatia/demon) 
    - Halo PC Demo re-implementation project.
- [Ringhopper](https://github.com/FishAndRips/ringhopper)
    - A toolkit for creating and manipulating tag data for Halo CE.
- [Chimera](https://github.com/SnowyMouse/chimera) 
    - A mod for Halo Custom Edition that adds new features and fixes bugs.
- [OpenSauce](https://github.com/MirisWisdom/OpenSauce) 
    - Another mod for Halo Custom Edition that extends the game's engine capabilities.
- [C20](https://c20.reclaimers.net/) 
    - A wiki with a lot of knowledge about Halo CE.
- [butcher.doc](http://game-tech.com/Talks/Butcher.doc) 
    - A document that seems to be some kind of report about the Halo 2 engine.
- [Tempera](https://github.com/Dwood15/Tempera) 
    - Another reverse engineering project for Halo PC.
- [halo-re](https://github.com/halo-re/halo) 
    - A project that aims to reverse engineer the original release of Halo: Combat Evolved on Xbox.
