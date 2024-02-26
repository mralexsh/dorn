# Directories
B_DIR=build
INCLUDES=
LIBS=

# Files
S_FILES=*.c 

# Output
EXEC=$(B_DIR)/dorn

# Build settings
CC=clang
# SDL options
CC_SDL=
CFLAGS = -Wall -g -lm

all:Build

Build:
	$(CC) $(CFLAGS) $(S_FILES) $(INCLUDES) $(LIBS) -o $(EXEC) $(CC_SDL)

build_run:Build
	$(EXEC)

clean:
	rm -rf $(B_DIR)/*
