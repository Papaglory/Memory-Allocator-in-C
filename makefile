# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -O2

# Directories
SRC_DIR = src
OBJ_DIR = build

# Find all source files
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Default target
all: $(OBJ_FILES)

# Rule to create object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean

