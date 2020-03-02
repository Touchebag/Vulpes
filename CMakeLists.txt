cmake_minimum_required(VERSION 3.0)
project(vulpes)

set(CMAKE_INSTALL_PREFIX build)

# Targets
set(GAME_TARGET game)
set(LEVEL_EDITOR_TARGET level_editor)
set(MAIN_TARGET vulpes)
set(STATE_GRAPH_TARGET state_graph)
set(TEST_TARGET tests)

set(CC_OPTIONS -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wold-style-cast -DLOG_LEVEL=3)

set(ASSETS_DIR assets)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SFML
set(BUILD_SHARED_LIBS false)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_LIBS sfml-graphics sfml-window sfml-system)

# GTest
find_package(GTest REQUIRED)

# Assets
add_custom_target(COPY_ASSETS)
add_custom_command(TARGET COPY_ASSETS PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                     copy_directory ${CMAKE_SOURCE_DIR}/${ASSETS_DIR} ${ASSETS_DIR})

add_custom_target(COPY_TEST_ASSETS)
file(GLOB_RECURSE TEST_ASSETS test/test_data/rooms/*.json)
foreach(file ${TEST_ASSETS})
    add_custom_command(TARGET COPY_TEST_ASSETS PRE_BUILD
                         COMMAND ${CMAKE_COMMAND} -E
                         copy ${file} ${ASSETS_DIR}/rooms/)
endforeach()

# Game target
file(GLOB_RECURSE COMMON_SOURCES src/game/*.cpp)
add_library(${GAME_TARGET} STATIC ${COMMON_SOURCES})
target_compile_options(${GAME_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${GAME_TARGET} ${SFML_LIBS})

# Level editor target
file(GLOB_RECURSE LEVEL_EDITOR_SOURCES src/level_editor/*.cpp)
add_library(${LEVEL_EDITOR_TARGET} ${LEVEL_EDITOR_SOURCES})
target_compile_options(${LEVEL_EDITOR_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${LEVEL_EDITOR_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${LEVEL_EDITOR_TARGET} ${GAME_TARGET} ${SFML_LIBS})

# Main target
file(GLOB_RECURSE GAME_SOURCES src/main/main.cpp)
add_executable(${MAIN_TARGET} ${GAME_SOURCES})
target_compile_options(${MAIN_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${MAIN_TARGET} PUBLIC src/ libs/nlohmann)

target_link_libraries(${MAIN_TARGET} ${GAME_TARGET} ${LEVEL_EDITOR_TARGET} ${SFML_LIBS})

add_dependencies(${MAIN_TARGET} COPY_ASSETS)

# State editor target
# TODO Broken since game/level editor merge. Fix when adding enemies
# file(GLOB_RECURSE STATE_GRAPH_SOURCES src/state_graph/*.cpp)
# add_executable(${STATE_GRAPH_TARGET} EXCLUDE_FROM_ALL ${STATE_GRAPH_SOURCES})
# target_compile_options(${STATE_GRAPH_TARGET} PUBLIC ${CC_OPTIONS})
# target_include_directories(${STATE_GRAPH_TARGET} PUBLIC src/game/ libs/nlohmann)
#
# target_link_libraries(${STATE_GRAPH_TARGET} ${COMMON_TARGET} ${SFML_LIBS})

# Test target
file(GLOB_RECURSE TEST_SOURCES test/src/*.cpp)
# Remove mocked classes
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/level_editor/mouse.cpp)
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/level_editor/main.cpp)

add_executable(${TEST_TARGET} ${TEST_SOURCES})
target_compile_options(${TEST_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${TEST_TARGET} PUBLIC src/ libs/nlohmann ${GTEST_INCLUDE_DIRS})

target_link_libraries(${TEST_TARGET} ${GAME_TARGET} ${LEVEL_EDITOR_TARGET} ${SFML_LIBS} ${GTEST_BOTH_LIBRARIES})

add_dependencies(${TEST_TARGET} COPY_TEST_ASSETS)

