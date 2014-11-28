# Settings
#DEBUG = -g
CFLAGS = -Wall -std=c++11 -O3 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Files
BUILDDIR = build
TARGET = puzzle
TEST_TARGET = $(BUILDDIR)/test

CPPS = src/puzzle.cpp
MAIN = src/main.cpp
TEST = src/test.cpp
HEADERS = puzzle.hpp fraction.hpp
HPPS = $(patsubst %,src/%,$(HEADERS))

MAIN_OBJS = $(patsubst src/%.cpp,$(BUILDDIR)/%.o,$(CPPS) $(MAIN))
TEST_OBJS = $(patsubst src/%.cpp,$(BUILDDIR)/%.o,$(CPPS) $(TEST))

# Main target
$(TARGET): $(BUILDDIR)/ $(MAIN_OBJS)
	$(CXX) $(LFLAGS) -o $@ $(MAIN_OBJS)

# Test binary
$(TEST_TARGET): $(BUILDDIR)/ $(TEST_OBJS)
	$(CXX) $(LFLAGS) -lboost_unit_test_framework -o $@ $(TEST_OBJS)

# Object files
$(BUILDDIR)/%.o: src/%.cpp $(HPPS)
	$(CXX) -c $(CFLAGS) -o $@ $<

$(BUILDDIR)/:
	mkdir $(BUILDDIR)

# Tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	-rm $(BUILDDIR)/*.o $(TARGET) $(TEST_TARGET)

.PHONY: test clean
