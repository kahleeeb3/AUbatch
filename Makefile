# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# -I. is the current directory
# $@ is the target name for the current rule
# $^ is the entire dependencies list
# $< is the first item in the dependencies list

CC=gcc# the compiler we will use
IDIR =./include

# CONFIG FILE
CFLAGS=-lpthread -Wall -I$(IDIR)

# name of the executable file
TARGET = aubatch
# list the header file names
INC_LIST = commandline_parser.h commands.h
# list the object file names
OBJ_LIST = aubatch.o commandline_parser.o commands.o

OBJ_DIR=obj# directory for storing object files
SRC_DIR=src# directory for storing source code
INC_DIR=include# directory for storing header files

# add directories to the header and object file names
INCS = $(patsubst %,$(INC_DIR)/%,$(INC_LIST)) # add INC_DIR to INC_LIST
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJ_LIST)) # add OBJ_DIR to OBJ_LIST

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean # keeps make from doing something with a file named clean

clean: #remove object files and executable
	rm -f $(OBJ_DIR)/*.o *~ core $(TARGET)

run: # run the executable
	./$(TARGET)