CC = g++

TARGET = main.exe

INC_DIRS = src SFML/include
LIB_DIRS = SFML/lib
LIBS = sfml-graphics sfml-window sfml-system

INC_PARAMS=$(foreach d, $(INC_DIRS), -I$d)
LIB_DIR_PARAMS=$(foreach d, $(LIB_DIRS), -L$d)
LIB_PARAMS=$(foreach d, $(LIBS), -l$d)

$(TARGET): src/main.cpp
	$(CC) $< $(INC_PARAMS) $(LIB_DIR_PARAMS) $(LIB_PARAMS) -o $@
