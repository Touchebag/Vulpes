CC = g++
CPPFLAGS = -MMD -MP -Wall -Wpedantic -std=c++17
LOG_LEVEL = 0

TARGET = out.exe

INC_DIRS = src libs/SFML/include libs/nlohmann
LIB_DIRS = libs/SFML/lib
BUILD_DIR = build

LIBS = sfml-graphics sfml-window sfml-system
SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%,$(BUILD_DIR)/%,$(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

INC_PARAMS=$(foreach d, $(INC_DIRS), -I$d)
LIB_DIR_PARAMS=$(foreach d, $(LIB_DIRS), -L$d)
LIB_PARAMS=$(foreach d, $(LIBS), -l$d)

LOG_LEVEL_PARAM = -DLOG_LEVEL=$(LOG_LEVEL)

LDFLAGS = $(CPPFLAGS) $(LOG_LEVEL_PARAM) $(INC_PARAMS)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIB_DIR_PARAMS) $(LIB_PARAMS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

.PHONY: clean
