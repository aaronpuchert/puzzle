#include "puzzle.hpp"
#include <sstream>
#include <gtest/gtest.h>

class PuzzleTest : public testing::TestWithParam<const char*> {};

static testing::AssertionResult verifySolutions(
	const char* /* solver_expr */, const char* numSol_expr,
	Puzzle::PuzzleSolver &solver, int numSol)
{
	std::ostringstream str;
	int actSol = solver.print_solutions(str, true);
	if (actSol == numSol)
		return testing::AssertionSuccess();
	else
		return testing::AssertionFailure()
			<< "Expected: " << numSol << " solution(s) (" << numSol_expr << ")\n"
			<< "  Actual: " << actSol << " solution(s)\n" << str.str();
}

TEST_P(PuzzleTest, Solve)
{
	Puzzle::Puzzle puzzle(GetParam(), 10);
	Puzzle::PuzzleSolver solver(puzzle);
	EXPECT_PRED_FORMAT2(verifySolutions, solver, 1);
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

INSTANTIATE_TEST_CASE_P(PureTests, PuzzleTest, ::testing::ValuesIn(puzzles));
