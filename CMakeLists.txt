cmake_minimum_required(VERSION 3.0)
project(vulpes)

set(CMAKE_INSTALL_PREFIX build)

# Targets
set(GAME_LIB_TARGET game_lib)
set(GAME_TARGET vulpes)
set(EDITOR_LIB_TARGET editor_lib)
set(EDITOR_TARGET level_editor)
set(STATE_GRAPH_TARGET state_graph)
set(TEST_TARGET tests)

set(CC_OPTIONS -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wold-style-cast -Werror=suggest-override -DLOG_LEVEL=3)

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
add_dependencies(COPY_TEST_ASSETS COPY_ASSETS)

# Game library
file(GLOB_RECURSE GAME_LIB_SOURCES src/game/*.cpp)
list(REMOVE_ITEM GAME_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/game/main.cpp)

add_library(${GAME_LIB_TARGET} STATIC ${GAME_LIB_SOURCES})
target_compile_options(${GAME_LIB_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_LIB_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${GAME_LIB_TARGET} ${SFML_LIBS})

# Game target
file(GLOB_RECURSE GAME_SOURCES src/game/main.cpp)
add_executable(${GAME_TARGET} ${GAME_SOURCES})
target_compile_options(${GAME_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_TARGET} PUBLIC src/ libs/nlohmann)

target_link_libraries(${GAME_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS})

add_dependencies(${GAME_TARGET} COPY_ASSETS)

# Level editor library
file(GLOB_RECURSE EDITOR_LIB_SOURCES src/level_editor/*.cpp)
list(REMOVE_ITEM EDITOR_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/level_editor/main.cpp)
add_library(${EDITOR_LIB_TARGET} ${EDITOR_LIB_SOURCES})
target_compile_options(${EDITOR_LIB_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${EDITOR_LIB_TARGET} PUBLIC src/game/ src/level_editor/ libs/nlohmann)

target_link_libraries(${EDITOR_LIB_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS})

# Level editor target
file(GLOB_RECURSE EDITOR_SOURCES src/level_editor/main.cpp)
add_executable(${EDITOR_TARGET} ${EDITOR_SOURCES})
target_compile_options(${EDITOR_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${EDITOR_TARGET} PUBLIC src/game/ libs/nlohmann)

target_link_libraries(${EDITOR_TARGET} ${EDITOR_LIB_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS})

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

target_link_libraries(${TEST_TARGET} ${GAME_LIB_TARGET} ${EDITOR_LIB_TARGET} ${SFML_LIBS} ${GTEST_BOTH_LIBRARIES})

add_dependencies(${TEST_TARGET} COPY_TEST_ASSETS)

