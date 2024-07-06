# Compiler to use
CC=gcc

# Compiler flags
CFLAGS=-Wall -Wno-unused-variable -Wno-unused-value

# List of source files
SRCS=main.c saveToHistoryFile.c  PipeCommands.c StringToArray.c ExecuteCommands.c CommandWithAnd.c FileStringToArray.c ExecuteFileCommands.c  infinite_input.c  DoBonus.c

# Name of the final executable
TARGET=outputProgram

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
