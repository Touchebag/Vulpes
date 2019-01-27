CC = g++
CPPFLAGS = -MMD -MP -Wall -Wpedantic -std=c++17

LOG_LEVEL = 3
LOG_LEVEL_PARAM = -DLOG_LEVEL=$(LOG_LEVEL)

LDFLAGS = $(CPPFLAGS) $(LOG_LEVEL_PARAM)

#### COMMON BUILD OPTIONS ####

COMMON_INC_DIRS = src/common libs/SFML/include libs/nlohmann
COMMON_LIB_DIRS = libs/SFML/lib
COMMON_BUILD_DIR = build/common

COMMON_LIBS = sfml-graphics sfml-window sfml-system
COMMON_SRCS = $(wildcard src/common/*.cpp)
COMMON_OBJS = $(patsubst src/common/%,$(COMMON_BUILD_DIR)/%,$(COMMON_SRCS:.cpp=.o))
COMMON_DEPS = $(COMMON_OBJS:.o=.d)

COMMON_INC_PARAMS=$(foreach d, $(COMMON_INC_DIRS), -I$d)
COMMON_LIB_DIR_PARAMS=$(foreach d, $(COMMON_LIB_DIRS), -L$d)
COMMON_LIB_PARAMS=$(foreach d, $(COMMON_LIBS), -l$d)


#### GAME ###

GAME_TARGET = vulpes.exe

GAME_INC_DIRS = $(COMMON_INC_DIRS) src/game
GAME_LIB_DIRS = $(COMMON_LIB_DIRS)
GAME_BUILD_DIR = build/game

GAME_LIBS = $(COMMON_LIBS)
GAME_SRCS = $(COMMON_SRCS) $(wildcard src/game/*.cpp)
GAME_OBJS = $(COMMON_OBJS) $(patsubst src/game/%,$(GAME_BUILD_DIR)/%,$(GAME_SRCS:.cpp=.o))
GAME_DEPS = $(COMMON_OBJS:.o=.d)

GAME_INC_PARAMS = $(foreach d, $(GAME_INC_DIRS), -I$d)
GAME_LIB_DIR_PARAMS = $(foreach d, $(GAME_LIB_DIRS), -L$d)
GAME_LIB_PARAMS = $(foreach d, $(GAME_LIBS), -l$d)


#### STATE_EDITOR ####

STATE_EDITOR_TARGET = state_editor.exe

STATE_EDITOR_INC_DIRS = $(COMMON_INC_DIRS) src/state_editor
STATE_EDITOR_LIB_DIRS = $(COMMON_LIB_DIRS)
STATE_EDITOR_BUILD_DIR = build/state_editor

STATE_EDITOR_LIBS = $(COMMON_LIBS)
STATE_EDITOR_SRCS = $(COMMON_SRCS) $(wildcard src/state_editor/*.cpp)
STATE_EDITOR_OBJS = $(COMMON_OBJS) $(patsubst src/state_editor/%,$(STATE_EDITOR_BUILD_DIR)/%,$(STATE_EDITOR_SRCS:.cpp=.o))
STATE_EDITOR_DEPS = $(COMMON_OBJS:.o=.d)

STATE_EDITOR_INC_PARAMS = $(foreach d, $(STATE_EDITOR_INC_DIRS), -I$d)
STATE_EDITOR_LIB_DIR_PARAMS = $(foreach d, $(STATE_EDITOR_LIB_DIRS), -L$d)
STATE_EDITOR_LIB_PARAMS = $(foreach d, $(STATE_EDITOR_LIBS), -l$d)


#### BUILD TARGETS ####

default: game

game: $(GAME_TARGET)
state_editor: $(STATE_EDITOR_TARGET)

$(GAME_TARGET): $(GAME_OBJS)
	echo $(GAME_OBJS)
	$(CC) $(LDFLAGS) $(GAME_INC_PARAMS) $(GAME_OBJS) -o $@ $(GAME_LIB_DIR_PARAMS) $(GAME_LIB_PARAMS)

$(STATE_EDITOR_TARGET): $(STATE_EDITOR_OBJS)
	echo $(STATE_EDITOR_OBJS)
	$(CC) $(LDFLAGS) $(STATE_EDITOR_INC_PARAMS) $(STATE_EDITOR_OBJS) -o $@ $(STATE_EDITOR_LIB_DIR_PARAMS) $(STATE_EDITOR_LIB_PARAMS)


#### BUILD ARTIFACTS ####

$(COMMON_BUILD_DIR)/%.o: src/common/%.cpp
	@mkdir -p $(COMMON_BUILD_DIR)
	$(CC) $(LDFLAGS) $(COMMON_INC_PARAMS) -c $< -o $@

$(GAME_BUILD_DIR)/%.o: src/game/%.cpp
	@mkdir -p $(GAME_BUILD_DIR)
	$(CC) $(LDFLAGS) $(GAME_INC_PARAMS) -c $< -o $@

$(STATE_EDITOR_BUILD_DIR)/%.o: src/state_editor/%.cpp
	@mkdir -p $(STATE_EDITOR_BUILD_DIR)
	$(CC) $(LDFLAGS) $(STATE_EDITOR_INC_PARAMS) -c $< -o $@


#### DEPENDENCY FILES ####

-include $(COMMON_DEPS)
-include $(GAME_DEPS)
-include $(STATE_EDITOR_DEPS)


clean:
	$(RM) $(COMMON_OBJS)
	$(RM) $(GAME_OBJS)
	$(RM) $(GAME_TARGET)
	$(RM) $(STATE_EDITOR_OBJS)
	$(RM) $(STATE_EDITOR_TARGET)

.PHONY: clean
