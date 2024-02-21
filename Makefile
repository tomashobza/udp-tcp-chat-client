CXX=g++-13
CXXFLAGS=-std=c++20 -Wall -Wextra -pedantic

.PHONY: main run clean server

main: main.cpp
	@mkdir -p bin
	$(CXX) -o bin/main main.cpp $(CXXFLAGS)

run: main
	@echo
	@./bin/main

clean:
	rm -f hello

server:
	nodemon --exec python3 python/server.py
