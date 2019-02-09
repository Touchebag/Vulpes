cmake_minimum_required(VERSION 3.0)
project(vulpes)

set(CMAKE_INSTALL_PREFIX build)

set(COMMON_TARGET common)
set(GAME_TARGET vulpes)
set(STATE_EDITOR_TARGET state_editor)

set(CC_OPTIONS -std=c++17 -Wall -Wpedantic -DLOG_LEVEL=3)

# SFML
# TODO Change to static
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_LIBS sfml-graphics sfml-window sfml-system)

# Assets
file(COPY assets DESTINATION .)
file(COPY sfml-graphics-d-2.dll DESTINATION .)
file(COPY sfml-window-d-2.dll DESTINATION .)
file(COPY sfml-system-d-2.dll DESTINATION .)

# Common library
file(GLOB_RECURSE COMMON_SOURCES src/common/*.cpp src/common/*.h)
add_library(${COMMON_TARGET} STATIC ${COMMON_SOURCES})
target_compile_options(${COMMON_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${COMMON_TARGET} PUBLIC libs/nlohmann)

# Game target
file(GLOB_RECURSE GAME_SOURCES src/game/*.cpp src/game/*.h)
add_executable(${GAME_TARGET} ${GAME_SOURCES})
target_compile_options(${GAME_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_TARGET} PUBLIC src/common/ libs/nlohmann)

target_link_libraries(${GAME_TARGET} ${COMMON_TARGET} ${SFML_LIBS})

# State editor target
file(GLOB_RECURSE STATE_EDITOR_SOURCES src/state_editor/*.cpp src/state_editor/*.h)
add_executable(${STATE_EDITOR_TARGET} ${STATE_EDITOR_SOURCES})
target_compile_options(${STATE_EDITOR_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${STATE_EDITOR_TARGET} PUBLIC src/common/ libs/nlohmann)

target_link_libraries(${STATE_EDITOR_TARGET} ${COMMON_TARGET} ${SFML_LIBS})

