# Makefile
# --------
# Builds the Indexed Allocation File System simulation.
#
# Usage:
#   make clean    - remove all compiled object files and the executable
#   make run      - build (if needed) then run the executable
#
# Flags:
#   -Wall -Wextra - enable all standard and extra compiler warnings
#   -std=c11      - compile to the C11 standard
#   -g            - include debug symbols for use with gdb / valgrind

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = fs_sim
SRCS    = fs_indexed.c main.c
OBJS    = $(SRCS:.c=.o)       # derive object file list from source file list
HEADERS = fs_indexed.h

.PHONY: all run clean

# ── Default target: build the executable ─────────────────────────────────────
all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ── Pattern rule: compile each .c file into a .o file ────────────────────────
# Listing the header as a prerequisite ensures a rebuild if the header changes
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# ── Build then immediately run ────────────────────────────────────────────────
run: all
	./$(TARGET)

# ── Remove all build artefacts ────────────────────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)
