# Compiler and flags
CC = cl
CFLAGS = /Iinclude /EHsc /Zi
# CFLAGS = /Iinclude /EHsc

# Libraries
LIBS = user32.lib shell32.lib gdi32.lib

# Source files
SRCS = systray.c
SRCS += $(wildcard include/*.c)

# Output binary
TARGET = systray.exe

# Default rule
all: $(TARGET)

# Compile and link
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) /Fe:$(TARGET)

# Clean up
clean:
	del $(TARGET) *.obj
