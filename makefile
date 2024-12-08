# Compiler and flags
CC = cl
CFLAGS = /Iinclude /EHsc
# CFLAGS = /Iinclude /EHsc

# Libraries
LIBS = user32.lib shell32.lib gdi32.lib

# Source files
SRCS = systray.c
SRCS += $(wildcard include/*.c)

# Output binary
TARGET = systray.exe

# Output directory for build files
OUT_DIR = .\out

# Object files
OBJS = $(SRCS:%.c=$(OUT_DIR)/%.obj)

# Force PowerShell usage
SHELL = powershell.exe

# Default rule
all: $(TARGET)

# Compile and link
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) /Fe:$(TARGET)

# Rule to create object files
$(OUT_DIR)/%.obj: %.c
	# Ensure output directories exist
	@if (-not (Test-Path $(OUT_DIR))) { New-Item -ItemType Directory -Path $(OUT_DIR) }
	@if (-not (Test-Path $(OUT_DIR)/include)) { New-Item -ItemType Directory -Path $(OUT_DIR)/include }
	$(CC) /c $< $(CFLAGS) /Fo$@

# Clean up
clean:
	# Delete the output files and directory
	@if (Test-Path $(TARGET)) { Remove-Item $(TARGET) }
	@if (Test-Path $(OUT_DIR)) { Remove-Item -Recurse -Force $(OUT_DIR) }
