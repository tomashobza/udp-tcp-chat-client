CXX=g++-13
CXXFLAGS=-std=c++20 -Wall -Wextra -pedantic -g

# Define the target executable
TARGET=bin/ipk24chat-client

# Define source files
SOURCES=$(wildcard *.cpp)

# Define object files (not needed directly since we compile and link in one step here)
# OBJECTS=$(SOURCES:.cpp=.o)

.PHONY: main run clean server

# Main target
main: $(SOURCES)
	@mkdir -p bin
	$(CXX) -o $(TARGET) $(SOURCES) $(CXXFLAGS)

# Run target
run: main
	@echo "\n=== Running the program ==="
	@./$(TARGET)

test:
	python3 testing_server.py bin/ipk24chat-client

# Clean target
clean:
	rm -f $(TARGET)

# Server target
server:
	nodemon --exec python3 python/server.py
