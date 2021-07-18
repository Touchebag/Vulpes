# Vulpes
Custom game engine project.

## Building instructions

### Requirements
- cmake
- SFML development libraries
- Google test (optional)
- git-lfs (runtime assets)
- nlohmann-json

All of these (along with general c++ build tools) are available as packages on most Linux distributions. Windows users will have to add them to cmake environment manually (no instructions here currently).

Nlohmann-json can also be included manually as header-only by specifying the directory using the environment variable `JSON_LIB_DIR` (`$JSON_LIB_DIR/nlohmann/json.hpp`).

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

Press 'P' to enter/exit editor mode.

### Controls
#### Entity operations
- Left Mouse - Select entity under cursor
- Ctrl+Left Mouse - Move entity
- Shift+Left Mouse - Resize entity
- A - Add new entity
- C - Copy entity
- D - Delete entity
- Z - Undo
- R - Redo
- E - Open entity menu (E again to close)

#### Camera controls
- Alt+Left Mouse - Move view
- Alt+Right Mouse - Zoom view
- Mouse Scroll - Zoom view (alternate)
- Ctrl+Mouse Scroll (change active entity layer)
- V - Render current layer only
