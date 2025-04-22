# Compiler and flags
CC = cl
CI = ISCC.exe
CFLAGS = /Iinclude /EHsc

# Libraries
LIBS = user32.lib shell32.lib gdi32.lib

# Source files
SRCS = systray.c
SRCS += $(wildcard include/*.c)

ISRCS = .\install\installer.iss

# Resource file
RES = include/resource.rc

# Output binary
TARGET = WallCycle.exe

# Output directory for build files
OUT_DIR = .\out

# Object files
OBJS = $(SRCS:%.c=$(OUT_DIR)/%.obj)
RES_OBJ = $(OUT_DIR)/resource.res

# Force PowerShell usage
SHELL = powershell.exe

#release directory
RELEASE_DIR = .\release\src

#Animation directory
ANIMATION_DIR = ./include/src/Animation

# Default rule
all: $(TARGET)

# Compile and link
$(TARGET): $(OBJS) $(RES_OBJ)
	$(CC) $(OBJS) $(RES_OBJ) $(CFLAGS) $(LIBS) /Fe:$(TARGET)

# Rule to create object files
$(OUT_DIR)/%.obj: %.c
	# Ensure output directories exist
	@if (-not (Test-Path $(OUT_DIR))) { New-Item -ItemType Directory -Path $(OUT_DIR) }
	@if (-not (Test-Path $(OUT_DIR)/include)) { New-Item -ItemType Directory -Path $(OUT_DIR)/include }
	$(CC) /c $< $(CFLAGS) /Fo$@

# Rule to compile resource file
$(RES_OBJ): $(RES)
	# Ensure output directory exists
	@if (-not (Test-Path $(OUT_DIR))) { New-Item -ItemType Directory -Path $(OUT_DIR) }
	rc /fo $(RES_OBJ) $(RES)

# Clean up
clean:
	# Delete the output files and directory
	@if (Test-Path $(TARGET)) { Remove-Item $(TARGET) }
	@if (Test-Path $(OUT_DIR)) { Remove-Item -Recurse -Force $(OUT_DIR) }

installer:
	# Create the installer
	$(CI) $(ISRCS)

copy:
	@if (-not (Test-Path $(RELEASE_DIR))) { New-Item -ItemType Directory -Path $(RELEASE_DIR) }
	Copy-Item .\WallCycle.exe $(RELEASE_DIR)
	Copy-Item .\config.ini $(RELEASE_DIR)
	Copy-Item .\README.md $(RELEASE_DIR)

	@if (-not (Test-Path $(RELEASE_DIR)\img)) { New-Item -ItemType Directory -Path $(RELEASE_DIR)\img }
	Copy-Item -Recurse .\img\* $(RELEASE_DIR)\img\

release: all copy installer


release-clean: clean
	@if (Test-Path $(RELEASE_DIR)) { Remove-Item -Recurse -Force $(RELEASE_DIR) }

animation:
	# Create the animation frames from png
	./.venv/Scripts/activate
	python .\tooling\pngToIco.py -c "#1E1E1E" -f $(ANIMATION_DIR)
	Move-Item $(ANIMATION_DIR)\resource.h .\include\resource.h
	Move-Item $(ANIMATION_DIR)\resource.rc .\include\resource.rc

animation-clean:
	# Delete the animation frames
	Remove-Item $(ANIMATION_DIR)\*.ico
	Remove-Item .\include\resource.h
	Remove-Item .\include\resource.rc