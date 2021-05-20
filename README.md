# Vulpes
Custom game engine project.

## Building instructions

### Requirements
- cmake
- SFML development libraries
- Google test (optional)
- git-lfs (runtime assets) 

All of these (along with general c++ build tools) are available as packages on most Linux distributions. Windows users will have to add them to cmake environment manually (no instructions here currently).

Note: the art assets are stored using git-lfs so you will also have to run

    git lfs install
    git lfs pull

### Compiling

Execute from root of repo.

    mkdir build
    cd build
    cmake ..
    make <targets>

Make takes standard arguments (like -j for parallel builds). Available targets are:
- `vulpes` main game binary
- `level_editor` debug binary
- `tests` gtest suite

## Default controls

**Note** all actions are not available from start.

- Arrow keys - move
    - Up - enter doors
- Space - jump
- Left shift - dash
- Z - Attack

## Editor

Press 'P' to enter editor mode.

TODO Add controls
