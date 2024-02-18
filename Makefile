# Compiler and Compiler Flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -g

# Target executable name
TARGET = client

# Source files
SOURCES = $(wildcard *.c)

# Build target
$(TARGET): $(SOURCES)
	@$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)

run:
	@make
	@./$(TARGET)

.PHONY: $(TARGET) clean
