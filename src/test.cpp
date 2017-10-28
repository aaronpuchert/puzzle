#include "puzzle.hpp"
#include <iostream>
#define BOOST_TEST_MODULE SolverTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

int testPuzzle(const char *puzzle)
{
	Puzzle::Puzz puzz(puzzle, 10);
	Puzzle::PuzzleSolver solver(puzz);
	std::cout << "Solving " << puzzle << std::endl;
	return solver.print_solutions(std::cout, true);
}

const char *puzzles[] = {
	// Donald E. Knuth, The Art of Computer Programming, Vol. 4A, pp. 324--347
	"SEND+A+TAD+MORE=MONEY",
	"COUPLE+COUPLE=QUARTET",
	"SATURN+URANUS+NEPTUNE+PLUTO=PLANETS",
	"EARTH+AIR+FIRE+WATER=NATURE",
	"HIP*HIP=HURRAY",
	"PI*R*R=AREA",
	"NORTH/SOUTH=EAST/WEST",
	"TWENTY=SEVEN+SEVEN+SIX",
	"TWELVE+NINE+TWO=ELEVEN+SEVEN+FIVE",

	// ZEITmagazin, 19/2014, S. 44
	"JANUAR+FEBRUAR=STAUSEE",
	"MAERZ+APRIL=MELKEN",
	"MAI+JUNI+JULI=ALPIN",

	// ZEITmagazin, 37/2014, S. 95
	// Replaced umlauts, because puzzle works only with ASCII.
	"GABEL+LOFFEL=IRRWEGE",
	"GABEL+GABEL=ABZUGE",
	"GABEL+MESSER=DOLLAR",

	// ZEITmagazin, 6/2015
	"ZAUN+TUERE=MERLIN",
	"ZAUN+TUERE=ELSTER"
};

const char *special[] = {
	// nonpure
	"VIOLIN+VIOLIN+VIOLA=TRIO+SONATA",
	"TWO*TWO=SQUARE",

	// special condition: no zero
	"A/BC+D/EF+G/HI=1",
};

BOOST_AUTO_TEST_CASE(solver_test)
{
	for (int i = 0; i < (sizeof(puzzles)/sizeof(const char *)); ++i)
		BOOST_CHECK(testPuzzle(puzzles[i]) == 1);
}
