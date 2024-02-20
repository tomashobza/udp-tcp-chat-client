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

# Build targets
all: $(UDP_CLIENT) $(TCP_CLIENT)

$(UDP_CLIENT): udp-client.c
	@mkdir -p bin
	@$(CC) $(CFLAGS) udp-client.c $(wildcard udp/*.c) -o bin/$(UDP_CLIENT)

$(TCP_CLIENT): tcp-client.c
	@mkdir -p bin
	@$(CC) $(CFLAGS) tcp-client.c $(wildcard tcp/*.c) -o bin/$(TCP_CLIENT)

# Clean build files
clean:
	rm -f bin/$(UDP_CLIENT) bin/$(TCP_CLIENT)

build: clean all

# Run targets
run-udp: $(UDP_CLIENT)
	@./bin/$(UDP_CLIENT)

run-tcp: $(TCP_CLIENT)
	@./bin/$(TCP_CLIENT)

server:
	nodemon --exec python3 python/server.py

kill:
	kill -9 $(lsof -ti:12000,12001)

.PHONY: all clean run-udp run-tcp server kill
