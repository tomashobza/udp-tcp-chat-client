DEBUG = 0

# Compiler and Compiler Flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 
ifeq ($(DEBUG), 1)
	CFLAGS += -g
endif

# Output binaries
UDP_CLIENT = udp-client
TCP_CLIENT = tcp-client

# Source files
SOURCES = $(wildcard *.c)

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Build targets
all: $(UDP_CLIENT) $(TCP_CLIENT)

$(UDP_CLIENT): udp-client.c
	@$(CC) $(CFLAGS) udp-client.c -o bin/$(UDP_CLIENT)

$(TCP_CLIENT): tcp-client.c
	@$(CC) $(CFLAGS) tcp-client.c -o bin/$(TCP_CLIENT)

# Clean build files
clean:
	rm -f $(UDP_CLIENT) $(TCP_CLIENT) $(OBJECTS)

# Run targets
run-udp: $(UDP_CLIENT)
	@./bin/$(UDP_CLIENT)

run-tcp: $(TCP_CLIENT)
	@./bin/$(TCP_CLIENT)

server:
	nodemon --exec python3 python/server.py

.PHONY: all clean run-udp run-tcp server
