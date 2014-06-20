# Settings
CXX ?= clang++
#DEBUG = -g
CFLAGS = -Wall -O3 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Files
TARGET = puzzle
CPPS = puzzle.cpp main.cpp
OBJS = $(patsubst %.cpp, %.o, $(CPPS))

# Main target
$(TARGET): $(OBJS)
	$(CXX) $(LFLAGS) -o puzzle $(OBJS)

# Object files
$(OBJS): %.o: %.cpp puzzle.h fraction.h
	$(CXX) -c $(CFLAGS) -o $@ $<

# Tests
test: $(TARGET)
	./examples

clean:
	-rm *.o $(TARGET)

.PHONY: test clean
