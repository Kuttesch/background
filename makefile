# Compiler and flags
CC = cl
CFLAGS = /Iinclude /EHsc

# Libraries
LIBS = user32.lib shell32.lib gdi32.lib

# Source files
SRCS = systray.c
SRCS += $(wildcard include/*.c)

# Resource file
RES = include/resource.rc

# Output binary
TARGET = systray.exe

# Output directory for build files
OUT_DIR = .\out

# Object files
OBJS = $(SRCS:%.c=$(OUT_DIR)/%.obj)
RES_OBJ = $(OUT_DIR)/resource.res

# Force PowerShell usage
SHELL = powershell.exe

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