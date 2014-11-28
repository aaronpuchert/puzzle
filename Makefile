# Settings
#DEBUG = -g
CFLAGS = -Wall -std=c++11 -O3 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Files
BUILDDIR = build
TARGET = puzzle
SOURCES = puzzle.cpp main.cpp
HEADERS = puzzle.hpp fraction.hpp
CPPS = $(patsubst %,src/%,$(SOURCES))
HPPS = $(patsubst %,src/%,$(HEADERS))
OBJS = $(patsubst src/%.cpp,$(BUILDDIR)/%.o,$(CPPS))

# Main target
$(TARGET): $(BUILDDIR)/ $(OBJS)
	$(CXX) $(LFLAGS) -o $(TARGET) $(OBJS)

# Object files
$(OBJS): $(BUILDDIR)/%.o: src/%.cpp $(HPPS)
	$(CXX) -c $(CFLAGS) -o $@ $<

$(BUILDDIR)/:
	mkdir $(BUILDDIR)

# Tests
test: $(TARGET)
	./examples

clean:
	-rm $(OBJS) $(TARGET)

.PHONY: test clean
