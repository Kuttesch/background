# Compiler and flags
CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -mwindows

# Resource compiler
RC = windres

# Libraries
LIBS = -luser32 -lshell32 -lgdi32

# Directories
SRC_DIR = ./src
ASSETS_DIR = $(SRC_DIR)/assets
ANIMATION_DIR = $(ASSETS_DIR)/frames
INCLUDE_DIR = $(SRC_DIR)/include
INSTALLER_DIR = ./install
RELEASE_DIR = ./release/src
OUT_DIR = ./out

# Source files
SRCS = $(SRC_DIR)/systray.c
SRCS += $(wildcard $(INCLUDE_DIR)/*.c)

# Resource file
RES = $(INCLUDE_DIR)/resource.rc

# Output binary
TARGET = WallCycle.exe

# Object files
OBJS = $(SRCS:%.c=$(OUT_DIR)/%.o)
RES_OBJ = $(OUT_DIR)/resource.o

# Installer
CI = ISCC.exe
ISRCS = $(INSTALLER_DIR)/installer.iss

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
	mv $(ANIMATION_DIR)/resource.h $(INCLUDE_DIR)/resource.h && \
	mv $(ANIMATION_DIR)/resource.rc $(INCLUDE_DIR)/resource.rc

# Clean animation icons
animation-clean:
	rm -f $(ANIMATION_DIR)/*.ico
	rm -f $(INCLUDE_DIR)/resource.h
	rm -f $(INCLUDE_DIR)/resource.rc
