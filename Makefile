CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJ_DIR = obj
EXE = stegim.exe

# List of source files in different directories
SRCS = stegim.c modules/bmp_lib.c modules/png_lib.c modules/input.c modules/pixel_secrets.c modules/lzw.c

# Generate list of object files based on source files
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(EXE)

# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(OBJ_DIR)/modules:
	mkdir -p $(OBJ_DIR)/modules

# Compile source files into object files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR) $(OBJ_DIR)/modules
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into executable
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lpng
	
clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/modules/*.o $(EXE)
