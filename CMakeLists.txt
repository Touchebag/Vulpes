cmake_minimum_required(VERSION 3.11)
project(vulpes)

include(FetchContent)

set(CMAKE_INSTALL_PREFIX build)
IF (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
ENDIF()


# Targets
set(GAME_LIB_TARGET game_lib)
set(GAME_TARGET vulpes)
set(EDITOR_LIB_TARGET editor_lib)
set(EDITOR_TARGET editor)
set(TEST_TARGET tests)

set(CC_OPTIONS -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wold-style-cast -Wshadow -Werror=non-virtual-dtor -Werror=suggest-override -Werror=unused-result -DLOG_LEVEL=3)

set(ASSETS_DIR assets)
set(TOOLS_DIR tools)

set(NLOHMANN_JSON_VERSION v3.11.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Nlohmann json
FetchContent_Declare(
    json
    URL         https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH    SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
)

FetchContent_MakeAvailable(json)

add_library(json INTERFACE)
set(JSON_LIBS nlohmann_json::nlohmann_json)

# SFML
#set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_LIBS sfml-graphics sfml-window sfml-system)

# Assets
add_custom_target(COPY_ASSETS)
add_custom_command(TARGET COPY_ASSETS PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                     copy_directory ${CMAKE_SOURCE_DIR}/${ASSETS_DIR} ${ASSETS_DIR})

add_custom_target(COPY_TOOLS ALL)
add_custom_command(TARGET COPY_TOOLS POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                     copy ${CMAKE_SOURCE_DIR}/${TOOLS_DIR}/commit_check.sh ./commit_check.sh)

# Game library
file(GLOB_RECURSE GAME_LIB_SOURCES src/game/*.cpp)
list(REMOVE_ITEM GAME_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/game/main.cpp)

add_library(${GAME_LIB_TARGET} STATIC ${GAME_LIB_SOURCES})
target_compile_options(${GAME_LIB_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_LIB_TARGET} PUBLIC src/game/)

target_link_libraries(${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS})

# Game target
file(GLOB_RECURSE GAME_SOURCES src/game/main.cpp)
add_executable(${GAME_TARGET} ${GAME_SOURCES})
target_compile_options(${GAME_TARGET} PUBLIC ${CC_OPTIONS})
target_include_directories(${GAME_TARGET} PUBLIC src/ ${JSON_LIBS})

target_link_libraries(${GAME_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS})

add_dependencies(${GAME_TARGET} COPY_ASSETS)

# ImGui for the level editor
find_package(ImGui-SFML QUIET)
IF(ImGui-SFML_FOUND)
    # Add ImGui as system to avoid compiler warnings
    include_directories(SYSTEM libs/imgui)
    include_directories(SYSTEM ${CMAKE_PREFIX_PATH}/include)

    # Level editor library
    file(GLOB_RECURSE EDITOR_LIB_SOURCES src/editor/*.cpp)
    list(REMOVE_ITEM EDITOR_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/main.cpp)
    add_library(${EDITOR_LIB_TARGET} ${EDITOR_LIB_SOURCES})
    target_compile_options(${EDITOR_LIB_TARGET} PUBLIC ${CC_OPTIONS})

    target_include_directories(${EDITOR_LIB_TARGET} PUBLIC src/game/ src/editor/ ${JSON_INCLUDE_DIRS})
    target_link_libraries(${EDITOR_LIB_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS} ImGui-SFML::ImGui-SFML)

    # Level editor target
    file(GLOB_RECURSE EDITOR_SOURCES src/editor/main.cpp)
    add_executable(${EDITOR_TARGET} ${EDITOR_SOURCES})
    target_compile_options(${EDITOR_TARGET} PUBLIC ${CC_OPTIONS})

    target_include_directories(${EDITOR_TARGET} PUBLIC src/game/ ${JSON_INCLUDE_DIRS})
    target_link_libraries(${EDITOR_TARGET} ${EDITOR_LIB_TARGET} ${GAME_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS} ImGui-SFML::ImGui-SFML)
ELSE()
    message(STATUS "ImGui-SFML not found, skipping editor target")
ENDIF()

# GTest
find_package(GTest QUIET)
IF (GTest_FOUND)
    add_custom_target(COPY_TEST_ASSETS)
    add_custom_command(TARGET COPY_TEST_ASSETS PRE_BUILD
                         COMMAND ${CMAKE_COMMAND} -E
                         copy_directory ${CMAKE_SOURCE_DIR}/test/test_assets ${ASSETS_DIR})
    add_dependencies(COPY_TEST_ASSETS COPY_ASSETS)

    # Test target
    file(GLOB_RECURSE TEST_SOURCES test/src/*.cpp)
    # Remove mocked classes
    list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/mouse.cpp)
    list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/main.cpp)

    add_executable(${TEST_TARGET} ${TEST_SOURCES})
    target_compile_options(${TEST_TARGET} PUBLIC ${CC_OPTIONS})
    target_include_directories(${TEST_TARGET} PUBLIC src/ ${JSON_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

    target_link_libraries(${TEST_TARGET} ${GAME_LIB_TARGET} ${EDITOR_LIB_TARGET} ${SFML_LIBS} ${JSON_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_dependencies(${TEST_TARGET} COPY_TEST_ASSETS)
ELSE()
    message(STATUS "GTest not found, skipping tests target")
ENDIF()

