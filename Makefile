# Settings
VARIANT ?= release
ifeq ($(VARIANT),release)
ADDITIONAL_FLAGS = -O2 -DNDEBUG
else ifeq ($(VARIANT),debug)
ADDITIONAL_FLAGS = -ggdb3
else
ADDITIONAL_FLAGS = $(error Unknown variant, set VARIANT={debug|release})
endif
CXXFLAGS = -Wall -Wextra -std=c++11 $(ADDITIONAL_FLAGS)
LFLAGS = -Wall $(DEBUG)

# Files
BUILDDIR = $(VARIANT)
TARGET = $(BUILDDIR)/puzzle
TEST_TARGET = $(BUILDDIR)/test

CPPS = src/puzzle.cpp
MAIN = src/main.cpp
TEST = src/test.cpp
HEADERS = puzzle.hpp fraction.hpp
HPPS = $(patsubst %,src/%,$(HEADERS))

MAIN_OBJS = $(patsubst src/%.cpp,$(BUILDDIR)/%.o,$(CPPS) $(MAIN))
TEST_OBJS = $(patsubst src/%.cpp,$(BUILDDIR)/%.o,$(CPPS) $(TEST))

# Google Test shenanigans. Some distributions don't provide libgtest.so.
# So we have to compile it for ourselves first. Well that is fun.
ifdef GTEST_PREFIX
GTEST_DIR = $(GTEST_PREFIX)/src/gtest
GTEST_OBJ = $(BUILDDIR)/gtest-all.o $(BUILDDIR)/gtest_main.o
GTEST = $(GTEST_OBJ)
ifneq ($(GTEST_PREFIX),/usr)
CXXFLAGS += -I$(GTEST_PREFIX)/include
endif
$(GTEST_OBJ): $(BUILDDIR)/%.o: $(GTEST_DIR)/src/%.cc $(BUILDDIR)/
	$(CXX) -c $(CXXFLAGS) -I$(GTEST_DIR) -o $@ $^
else
GTEST = -lgtest -lgtest_main
endif

# Main target
$(TARGET): $(BUILDDIR)/ $(MAIN_OBJS)
	$(CXX) $(LFLAGS) -o $@ $(MAIN_OBJS)

# Test binary
$(TEST_TARGET): $(BUILDDIR)/ $(TEST_OBJS) $(GTEST_OBJ)
	$(CXX) $(LFLAGS) $(GTEST) -lpthread -o $@ $(TEST_OBJS)

# Object files
$(BUILDDIR)/%.o: src/%.cpp $(HPPS) $(BUILDDIR)/
	$(CXX) -c $(CXXFLAGS) -o $@ $<

$(BUILDDIR)/:
	mkdir $(BUILDDIR)

# Tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	-rm $(BUILDDIR)/*.o $(TARGET) $(TEST_TARGET)

.PHONY: test clean
