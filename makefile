# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Wall -mwindows

# Resource compiler
RC = windres

# Libraries
LIBS = -luser32 -lshell32 -lgdi32

# Source files
SRCS = systray.c
SRCS += $(wildcard include/*.c)

# Resource file
RES = include/resource.rc

# Output binary
TARGET = WallCycle.exe

# Output directory for build files
OUT_DIR = ./out

# Object files
OBJS = $(SRCS:%.c=$(OUT_DIR)/%.o)
RES_OBJ = $(OUT_DIR)/resource.o

# Installer
CI = ISCC.exe
ISRCS = ./install/installer.iss

# Release directory
RELEASE_DIR = ./release/src

# Animation directory
ANIMATION_DIR = ./include/src/Animation

# Default rule
all: $(TARGET)

# Compile and link
$(TARGET): $(OBJS) $(RES_OBJ)
	$(CC) $(OBJS) $(RES_OBJ) $(CFLAGS) $(LIBS) -o $(TARGET)

# Compile source files
$(OUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< $(CFLAGS) -o $@

# Compile resource file
$(RES_OBJ): $(RES)
	@mkdir -p $(OUT_DIR)
	$(RC) -i $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OUT_DIR) $(TARGET)

# Create installer
installer:
	$(CI) $(ISRCS)

# Copy release files
copy:
	@mkdir -p $(RELEASE_DIR)/img
	cp ./WallCycle.exe $(RELEASE_DIR)
	cp ./config.ini $(RELEASE_DIR)
	cp ./README.md $(RELEASE_DIR)
	cp -r ./img/* $(RELEASE_DIR)/img/

# Release task
release: all copy installer

# Clean release output
release-clean: clean
	rm -rf $(RELEASE_DIR)

# Generate animation icons and resources
animation:
	source ./.venv/Scripts/activate && \
	python ./tooling/pngToIco.py -c "#1E1E1E" -f $(ANIMATION_DIR) && \
	mv $(ANIMATION_DIR)/resource.h ./include/resource.h && \
	mv $(ANIMATION_DIR)/resource.rc ./include/resource.rc

# Clean animation icons
animation-clean:
	rm -f $(ANIMATION_DIR)/*.ico
	rm -f ./include/resource.h
	rm -f ./include/resource.rc
