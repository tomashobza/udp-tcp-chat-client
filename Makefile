CXX=g++-13
CXXFLAGS=-std=c++20 -Wall -Wextra -pedantic -g

# Define the target executable
TARGET=bin/ipk24chat-client

# Define source files
# SOURCES=$(wildcard src/*.cpp)
SOURCES=src/main.cpp src/ArgParser.cpp src/InputParser.cpp

.PHONY: main run clean server

# Main target
main: $(SOURCES)
	@mkdir -p bin
	$(CXX) -I./include -o $(TARGET) $(SOURCES) $(CXXFLAGS)

# Run target
run: main
	@echo "\n=== Running the program ==="
	@./$(TARGET) -t udp -s localhost

test:
	python3 python/testing_server.py bin/ipk24chat-client

# Clean target
clean:
	rm -f $(TARGET)

# Server target
server:
	nodemon --exec python3 python/server.py
