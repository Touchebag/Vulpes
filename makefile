CC = g++
CPPFLAGS =

TARGET = out.exe

INC_DIRS = src SFML/include
LIB_DIRS = SFML/lib
BUILD_DIR = build

LIBS = sfml-graphics sfml-window sfml-system
SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%,$(BUILD_DIR)/%,$(SRCS:.cpp=.o))

INC_PARAMS=$(foreach d, $(INC_DIRS), -I$d)
LIB_DIR_PARAMS=$(foreach d, $(LIB_DIRS), -L$d)
LIB_PARAMS=$(foreach d, $(LIBS), -l$d)

LDFLAGS = $(CPPFLAGS) $(INC_PARAMS)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIB_DIR_PARAMS) $(LIB_PARAMS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

.PHONY: clean
