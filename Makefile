# Settings
CXX ?= clang++
#DEBUG = -g
CFLAGS = -Wall -O3 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Files
TARGET = puzzle
SOURCES = puzzle.cpp main.cpp
HEADERS = puzzle.hpp fraction.hpp
CPPS = $(patsubst %,src/%,$(SOURCES))
HPPS = $(patsubst %,src/%,$(HEADERS))
OBJS = $(patsubst src/%.cpp,src/%.o,$(CPPS))

# Main target
$(TARGET): $(OBJS)
	$(CXX) $(LFLAGS) -o puzzle $(OBJS)

# Object files
$(OBJS): %.o: %.cpp $(HPPS)
	$(CXX) -c $(CFLAGS) -o $@ $<

# Tests
test: $(TARGET)
	./examples

clean:
	-rm src/*.o $(TARGET)

.PHONY: test clean
