CC = g++
CPPFLAGS =

TARGET = main.exe

INC_DIRS = src SFML/include
LIB_DIRS = SFML/lib

LIBS = sfml-graphics sfml-window sfml-system
SRCS = $(wildcard src/*.cpp)
OBJS = $(subst .cpp,.o,$(SRCS))

INC_PARAMS=$(foreach d, $(INC_DIRS), -I$d)
LIB_DIR_PARAMS=$(foreach d, $(LIB_DIRS), -L$d)
LIB_PARAMS=$(foreach d, $(LIBS), -l$d)

LDFLAGS = $(CPPFLAGS) $(INC_PARAMS)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIB_DIR_PARAMS) $(LIB_PARAMS)

%.o: %.cpp
	$(CC) $(LDFLAGS) -c $< -o $@

