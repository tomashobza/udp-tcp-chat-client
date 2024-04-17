CXX=clang++
CXXFLAGS=-std=c++2a -Wall -Wextra -pedantic -stdlib=libc++

# Define the target executable
TARGET=ipk24chat-client

# Define source files
SOURCES=$(wildcard src/*.cpp)

TEST_FLAGS=
ifeq ($(DEBUG), true)
	TEST_FLAGS=-d
	CXXFLAGS=-std=c++20 -Wall -Wextra -pedantic -g
endif

.PHONY: main run clean server docs diagrams zip

# Main target
main: $(SOURCES)
	$(CXX) -I./include -o $(TARGET) $(SOURCES) $(CXXFLAGS)

# Run target
run-udp: main
	@echo "\n=== Running the program ==="
	@./$(TARGET) -t udp -s localhost

run-tcp: main
	@echo "\n=== Running the program ==="
	@./$(TARGET) -t tcp -s localhost

run-only-udp:
	@./$(TARGET) -t udp -s localhost

run-only-tcp:
	@./$(TARGET) -t tcp -s localhost

test:
	python3 tests/testo.py ./$(TARGET) $(TEST_FLAGS)

test-tcp:
	python3 tests/testo.py ./$(TARGET) $(TEST_FLAGS) -p tcp

test-udp:
	python3 tests/testo.py ./$(TARGET) $(TEST_FLAGS) -p udp

docs:
	doxygen Doxyfile

diagrams:
	plantuml -tsvg -o ./ docs/*.puml'

zip:
	zip -r xhobza03.zip src/ include/ Makefile README.md CHANGELOG.md LICENSE

# Clean target
clean:
	rm -f $(TARGET)
	rm -rf tmp/

# Server target
server:
	nodemon --exec python3.11 python/server.py
