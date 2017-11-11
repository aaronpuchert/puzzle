#include <string>
#include <iostream>
#include <cstdlib>
#include "puzzle.hpp"

constexpr const char usage[] = R"#(
Finds all ways to replace letters by digits to satisfy the given equation.
The equation should be of the form expr=expr, where an expr is

    - a number in the given radix ([0-9]+),
    - a sequence of uppercase letters ([A-Z]+),
    - composites: expr+expr, expr-expr, expr*expr, expr/expr.

Different letters are replaced by different digits. Leading digits are not
allowed to be 0. The computation happens with 64-bit precision, and there is no
check for overflow.

If no radix is given, numbers are interpreted as decimal.
)#";

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 3) {
		std::cout << "Usage: " << argv[0] << " [radix] equation\n"
			<< usage << "\nExample: " << argv[0] << " SEND+MORE=MONEY\n";
		return 1;
	}

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
