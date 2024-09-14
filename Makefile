CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC_DIR = src
OBJ_DIR = build
SRC = $(wildcard $(SRC_DIR)/**/*.c) $(SRC_DIR)/main.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(OBJ)
	$(CC) $(OBJ) -o my_program

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) my_program

