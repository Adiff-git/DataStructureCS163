# Compiler and flags
CC = g++
CFLAGS = -I C:/raylib/w64devkit/x86_64-w64-mingw32/include
LDFLAGS = -L C:/raylib/w64devkit/x86_64-w64-mingw32/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Target executable
TARGET = my_program

# Source files
SRC = main.cpp

# Build target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Clean build files
clean:
	rm -f $(TARGET)
