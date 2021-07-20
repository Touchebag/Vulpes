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

set(CC_OPTIONS -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wold-style-cast -Wshadow -Werror=non-virtual-dtor -Werror=suggest-override -DLOG_LEVEL=3)

set(ASSETS_DIR assets)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Nlohmann json
IF(DEFINED ENV{JSON_LIB_DIR})
    set(JSON_INCLUDE_DIRS $ENV{JSON_LIB_DIR})
ELSE()
    find_package(nlohmann_json REQUIRED)
    set(JSON_LIBS nlohmann_json::nlohmann_json)
ENDIF()

# ImGui for the level editor
find_package(ImGui-SFML)

# SFML
set(BUILD_SHARED_LIBS false)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_LIBS sfml-graphics sfml-window sfml-system)

# GTest
find_package(GTest)

# Assets
add_custom_target(COPY_ASSETS)
add_custom_command(TARGET COPY_ASSETS PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                     copy_directory ${CMAKE_SOURCE_DIR}/${ASSETS_DIR} ${ASSETS_DIR})

add_custom_target(COPY_TEST_ASSETS)
add_custom_command(TARGET COPY_TEST_ASSETS PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                     copy_directory ${CMAKE_SOURCE_DIR}/test/test_assets ${ASSETS_DIR})
add_dependencies(COPY_TEST_ASSETS COPY_ASSETS)

# Game library
file(GLOB_RECURSE GAME_LIB_SOURCES src/game/*.cpp)
list(REMOVE_ITEM GAME_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/game/main.cpp)

add_library(${GAME_LIB_TARGET} STATIC ${GAME_LIB_SOURCES})
target_compile_options(${GAME_LIB_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_LIB_TARGET} PUBLIC src/game/ ${JSON_INCLUDE_DIRS})

target_link_libraries(${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS})

# Game target
file(GLOB_RECURSE GAME_SOURCES src/game/main.cpp)
add_executable(${GAME_TARGET} ${GAME_SOURCES})
target_compile_options(${GAME_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_TARGET} PUBLIC src/ ${JSON_INCLUDE_DIRS})

target_link_libraries(${GAME_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS})

add_dependencies(${GAME_TARGET} COPY_ASSETS)

IF(ImGui-SFML_FOUND)
    # Add ImGui as system to avoid compiler warnings
    include_directories(SYSTEM libs/imgui)
    include_directories(SYSTEM ${CMAKE_PREFIX_PATH}/include)

    # Level editor library
    file(GLOB_RECURSE EDITOR_LIB_SOURCES src/level_editor/*.cpp)
    list(REMOVE_ITEM EDITOR_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/level_editor/main.cpp)
    add_library(${EDITOR_LIB_TARGET} ${EDITOR_LIB_SOURCES})
    target_compile_options(${EDITOR_LIB_TARGET} PUBLIC ${CC_OPTIONS})

    target_include_directories(${EDITOR_LIB_TARGET} PUBLIC src/game/ src/level_editor/ ${JSON_INCLUDE_DIRS})
    target_link_libraries(${EDITOR_LIB_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS} ImGui-SFML::ImGui-SFML)

    # Level editor target
    file(GLOB_RECURSE EDITOR_SOURCES src/level_editor/main.cpp)
    add_executable(${EDITOR_TARGET} ${EDITOR_SOURCES})
    target_compile_options(${EDITOR_TARGET} PUBLIC ${CC_OPTIONS})

    target_include_directories(${EDITOR_TARGET} PUBLIC src/game/ ${JSON_INCLUDE_DIRS})
    target_link_libraries(${EDITOR_TARGET} ${EDITOR_LIB_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS} ImGui-SFML::ImGui-SFML)
ENDIF()

# State editor target
# TODO Broken since game/level editor merge. Fix when adding enemies
# file(GLOB_RECURSE STATE_GRAPH_SOURCES src/state_graph/*.cpp)
# add_executable(${STATE_GRAPH_TARGET} EXCLUDE_FROM_ALL ${STATE_GRAPH_SOURCES})
# target_compile_options(${STATE_GRAPH_TARGET} PUBLIC ${CC_OPTIONS})
# target_include_directories(${STATE_GRAPH_TARGET} PUBLIC src/game/ libs/nlohmann)
#
# target_link_libraries(${STATE_GRAPH_TARGET} ${COMMON_TARGET} ${SFML_LIBS})

if (GTest_FOUND)
    # Test target
    file(GLOB_RECURSE TEST_SOURCES test/src/*.cpp)
    # Remove mocked classes
    list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/level_editor/mouse.cpp)
    list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/level_editor/main.cpp)

    add_executable(${TEST_TARGET} ${TEST_SOURCES})
    target_compile_options(${TEST_TARGET} PUBLIC ${CC_OPTIONS})
    target_include_directories(${TEST_TARGET} PUBLIC src/ ${JSON_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

    target_link_libraries(${TEST_TARGET} ${GAME_LIB_TARGET} ${EDITOR_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_dependencies(${TEST_TARGET} COPY_TEST_ASSETS)
endif()

