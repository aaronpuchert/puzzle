#include "puzzle.hpp"
#include "util.hpp"
#include <cassert>
#include <utility>
#include <iterator>
#include <memory>
#include <limits>
#include <cstring>
#include <stdexcept>

namespace puzzle {

// BEGIN Implementation of ExpressionParser

/**
 * Parser data
 */
enum class NodeType {
	EQUAL,
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	LEAF
};

struct ExprType {
	char op;
	NodeType type;
	int priority;
};

static constexpr ExprType ParseTable[] = {
	{'=', NodeType::EQUAL, 0},
	{'+', NodeType::PLUS, 1},
	{'-', NodeType::MINUS, 1},
	{'*', NodeType::MULTIPLY, 2},
	{'/', NodeType::DIVIDE, 2}
};

Expr *ExpressionParser::parse(const char *expr)
{
	return parse(expr, expr + strlen(expr));
}

Expr *ExpressionParser::parse(const char *begin, const char *end)
{
	NodeType type = NodeType::LEAF;
	const char *split;
	int priority = std::numeric_limits<int>::max();

	// TODO: process parantheses
	// LOW: skip whitespace (well, maybe)
	for (const char *cur = begin; cur != end; ++cur)
		for (const ExprType& exprType : ParseTable)
			if (*cur == exprType.op && priority >= exprType.priority) {
				type = exprType.type;
				split = cur;
				priority = exprType.priority;
			}

	// split expression and process parts recursively
	if (type != NodeType::LEAF) {
		auto left = parse(begin, split);
		auto right = parse(split+1, end);
		switch (type) {
			using enum BinaryExpr::Op;

			case NodeType::EQUAL:
				return EqualityExpr::create(arena, left, right);
			case NodeType::PLUS:
				return BinaryExpr::create(arena, Add, left, right);
			case NodeType::MINUS:
				return BinaryExpr::create(arena, Sub, left, right);
			case NodeType::MULTIPLY:
				return BinaryExpr::create(arena, Mul, left, right);
			case NodeType::DIVIDE:
				return BinaryExpr::create(arena, Div, left, right);
			case NodeType::LEAF:
				PUZZLE_UNREACHABLE;
		}
		PUZZLE_UNREACHABLE;
	}
	else {
		// Is it a word or number?
		const char *cur;
		for (cur = begin; cur != end; ++cur)
			if (*cur >= '0' && *cur <= '9')
				break;
		if (cur == end) {
			size_t len = std::distance(begin, end);
			if (len > WordExpr::maxSize)
				throw std::out_of_range("Word too long");
			Letter word[WordExpr::maxSize];
			for (size_t i = 0; i < len; ++i)
				word[i] = letterToIndex.at(begin[(len-1) - i]);
			return WordExpr::create(arena, word, len);
		} else {
			int value = 0;
			for (const char *cur = begin; cur != end; ++cur) {
				value *= radix;
				value += *cur - '0';
			}
			return NumberExpr::create(arena, value);
		}
	}
}

// END Implementation of ExpressionParser

// BEGIN Implementation of Puzzle

Puzzle::Puzzle(const char *puzzle, int rad)
	: radix(rad), numLetters(0)
{
	// Collect letters.
	std::map<char, Letter> letterToIndex;
	for (const char *cur = puzzle; *cur; ++cur)
		if (*cur >= 'A' && *cur <= 'Z')
			letterToIndex[*cur];

	// Assign numbers to letters.
	for (auto &pair : letterToIndex) {
		assert(numLetters < maxNumLetters); // We only allow A-Z.
		indexToLetter[numLetters] = pair.first;
		pair.second = numLetters++;
	}

	// Make syntax tree.
	ExpressionParser parser(arena, letterToIndex, rad);
	root = parser.parse(puzzle);

	// Leading digits aren't allowed to be zero.
	if (puzzle[0] >= 'A' && puzzle[0] <= 'Z')
		leading[letterToIndex[puzzle[0]]] = true;
	for (int i = 1; puzzle[i]; ++i)
		if (puzzle[i-1] < 'A' && puzzle[i] >= 'A' && puzzle[i] <= 'Z')
			leading[letterToIndex[puzzle[i]]] = true;
}

// END Implementation of Puzzle

// BEGIN Implementation of Permutation generator

// The following algorithm is inspired by Donald E. Knuth: The Art of Computer
// Programming, Vol. 4, Fasc. 2, Algorithm L; but slightly modified.
// Algorithm M: Visit all injective maps f from {1, ..., m} to {1, ..., n},
// given m<n. Idea: Run through all m-subsets of {1, ..., n} and visit all
// permutations of these subsets for each one. Running through all m-subsets is
// done by a "combination-enumeration" algorithm which outputs all combinations
// in a canonical order. Running through all permutations of these subsets is
// done by Alg. L.
// M0. Start with map (a₁, ..., aₘ) = (1, ..., m).
// M1. Visit map (a₁, ..., aₘ)
// M2. [Find j] j ← m-1; while (aⱼ ≥ aⱼ₊₁}) --j;
//     if (j=0) goto M4;
// M3. [Next aⱼ] l ← n; while (aⱼ ≥ aₗ) --l; aⱼ ↔ aₗ;
// M4. [Reverse aⱼ₊₁}, ..., aₘ] k ← j + 1; l ← m;
//     while (k < l) {aₖ ↔ aₗ, k++, l--} if (j > 0) goto M1.
// M5. [Next combination] j ← m; while (aⱼ == j+(n-m)) --j;
//     if (j=0) finished;
//     else {++aⱼ; while (j<m) a₊₊ⱼ ← aⱼ₋₁ + 1; goto M1}

MapGen::MapGen(int domainSize, int codomainSize)
	: n(codomainSize), m(domainSize), map(new int[domainSize])
{
	if (codomainSize < domainSize)
		throw std::domain_error("There are no injective maps if the codomain "
		                        "is smaller than the domain.");

	// M0. Start with map (a₁, ..., aₘ) = (1, ..., m).
	for (int i = 0; i < domainSize; ++i)
		map[i] = i;
}

MapGen::~MapGen() = default;

bool MapGen::nextMap()
{
	// M2. Find j.
	int j = m - 2;  // "j ← m-1"
	while (j >= 0 && (map[j] >= map[j+1]))
		--j;
	if (j >= 0) {
		// M3. Next aⱼ.
		int l = m - 1;  // "l ← m"
		while (map[j] >= map[l])
			--l;
		std::swap(map[j], map[l]);
	}

	// M4. Reverse aⱼ₊₁, ..., aₘ.
	for (int k = j + 1, l = m - 1; k < l; ++k, --l)
		std::swap(map[k], map[l]);

	if (j < 0) {
		// M5. Next combination.
		j = m - 1;  // "j ← m"
		int diff = n-m;
		while (j >= 0 && (map[j] == j+diff))
			--j;
		if (j < 0)
			return false;
		else {
			int l = map[j];
			while (j < m)
				map[j++] = ++l;
		}
	}

	return true;
}

// END Implementation of Permutation generator

// BEGIN Implementation of Puzzle solver

PuzzleSolver::PuzzleSolver(const Puzzle &puzzle)
	: puzzle(puzzle) {}

int PuzzleSolver::print_solutions(std::ostream &out, bool terminal)
{
	int numSolutions = 0;

	try {
		MapGen mapGen(puzzle.getNumLetters(), puzzle.getRadix());

		if (terminal)
			out << "\e[1m";
		for (int i = 0; i < puzzle.getNumLetters(); ++i)
			out << puzzle[i] << ' ';
		if (terminal)
			out << "\e[0m";
		out << std::endl;

		GenericEvaluator eval(puzzle);
		do
			if (eval(*mapGen)) {
				++numSolutions;
				for (int i = 0; i < puzzle.getNumLetters(); ++i)
					out << mapGen[i] << ' ';
				out << std::endl;
			}
		while (mapGen.nextMap());
	}
	catch (const std::domain_error &) {
		out << "This alphametic has too many letters.\n\n";
	}

	return numSolutions;
}

// END Implementation of Puzzle solver.

}
