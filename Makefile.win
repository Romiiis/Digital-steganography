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

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lpng
	
clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/modules/*.o $(EXE)
