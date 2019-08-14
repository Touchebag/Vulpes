cmake_minimum_required(VERSION 3.0)
project(vulpes)

set(CMAKE_INSTALL_PREFIX build)

# Targets
set(COMMON_TARGET common)
set(GAME_TARGET vulpes)
set(STATE_GRAPH_TARGET state_graph)
set(LEVEL_EDITOR_TARGET level_editor)
set(TEST_TARGET tests)

set(CC_OPTIONS -std=c++17 -Wall -Wpedantic -Wconversion -Wold-style-cast -DLOG_LEVEL=3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SFML
set(BUILD_SHARED_LIBS false)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_LIBS sfml-graphics sfml-window sfml-system)

# GTest
find_package(GTest REQUIRED)

# Assets
file(COPY assets DESTINATION .)

# Export game files as lib for editors
file(GLOB_RECURSE COMMON_SOURCES src/game/*.cpp src/game/*.h)
add_library(${COMMON_TARGET} STATIC ${COMMON_SOURCES})
target_compile_options(${COMMON_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${COMMON_TARGET} PUBLIC src/game/ libs/nlohmann)

# Game target
file(GLOB_RECURSE GAME_SOURCES src/game/*.cpp src/game/*.h)
add_executable(${GAME_TARGET} ${GAME_SOURCES})
target_compile_options(${GAME_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${GAME_TARGET} ${SFML_LIBS})

# State editor target
file(GLOB_RECURSE STATE_GRAPH_SOURCES src/state_graph/*.cpp src/state_graph/*.h)
add_executable(${STATE_GRAPH_TARGET} EXCLUDE_FROM_ALL ${STATE_GRAPH_SOURCES})
target_compile_options(${STATE_GRAPH_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${STATE_GRAPH_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${STATE_GRAPH_TARGET} ${COMMON_TARGET} ${SFML_LIBS})

# Level editor target
file(GLOB_RECURSE LEVEL_EDITOR_SOURCES src/level_editor/*.cpp src/level_editor/*.h)
add_executable(${LEVEL_EDITOR_TARGET} ${LEVEL_EDITOR_SOURCES})
target_compile_options(${LEVEL_EDITOR_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${LEVEL_EDITOR_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${LEVEL_EDITOR_TARGET} ${COMMON_TARGET} ${SFML_LIBS})

# Test target
file(GLOB_RECURSE TEST_SOURCES test/*.cpp test/*.h)
add_executable(${TEST_TARGET} ${TEST_SOURCES})
target_compile_options(${TEST_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${TEST_TARGET} PUBLIC src/game/ libs/nlohmann ${GTEST_INCLUDE_DIRS})

target_link_libraries(${TEST_TARGET} ${COMMON_TARGET} ${SFML_LIBS} ${GTEST_BOTH_LIBRARIES})

