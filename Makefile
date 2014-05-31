# Definitions
OBJS = puzzle.o main.o
CXX = clang++
#DEBUG = -g
CFLAGS = -Wall -O3 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Main target
puzzle: $(OBJS)
	$(CXX) $(LFLAGS) -o puzzle $(OBJS)

# Object files
puzzle.o: puzzle.cpp puzzle.h
	$(CXX) -c $(CFLAGS) puzzle.cpp

main.o: main.cpp puzzle.h
	$(CXX) -c $(CFLAGS) main.cpp

# Tests
test: puzzle
	./examples

clean:
	-rm *.o puzzle
