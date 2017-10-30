#include <string>
#include <iostream>
#include <cstdlib>
#include "puzzle.hpp"

int main(int argc, char **argv)
{
	// extract puzzle out of command line
	int nRad = 10;
	if (argc > 2)	// then there is a radix argument
		nRad = atoi(argv[1]);

	Puzzle::Puzzle puzzle(argv[argc-1], nRad);
	std::cout << "There are " << puzzle.getNumLetters()
	          << " different letters.\n\n";

	Puzzle::PuzzleSolver solver(puzzle);

	int numSolutions = solver.print_solutions(std::cout, true);
	std::cout << numSolutions << " solutions found.\n";

	return 0;
}
