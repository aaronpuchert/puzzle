#include <utility>
#include <iterator>
#include <limits>
#include <cstring>
#include <stdexcept>
#include "puzzle.hpp"

/// We don't want to rely on C++14 yet.
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace Puzzle {

// BEGIN Implementation of Expressions

Expression::~Expression() = default;

class BinaryExpr : public Expression
{
protected:
	BinaryExpr(std::unique_ptr<Expression> left,
	           std::unique_ptr<Expression> right)
		: left(std::move(left)), right(std::move(right)) {}

	std::unique_ptr<Expression> left, right;
};

class EqualsExpr : public BinaryExpr
{
public:
	EqualsExpr(std::unique_ptr<Expression> left,
	           std::unique_ptr<Expression> right)
		: BinaryExpr(std::move(left), std::move(right)) {}

	fraction<int64_t> eval(const int *assignment) const override
		{ return left->eval(assignment) == right->eval(assignment); }
};

class SumExpr : public BinaryExpr
{
public:
	SumExpr(std::unique_ptr<Expression> left,
	        std::unique_ptr<Expression> right)
		: BinaryExpr(std::move(left), std::move(right)) {}

	fraction<int64_t> eval(const int *assignment) const override
		{ return left->eval(assignment) + right->eval(assignment); }
};

class DifferenceExpr : public BinaryExpr
{
public:
	DifferenceExpr(std::unique_ptr<Expression> left,
	               std::unique_ptr<Expression> right)
		: BinaryExpr(std::move(left), std::move(right)) {}

	fraction<int64_t> eval(const int *assignment) const override
		{ return left->eval(assignment) - right->eval(assignment); }
};

class ProductExpr : public BinaryExpr
{
public:
	ProductExpr(std::unique_ptr<Expression> left,
	            std::unique_ptr<Expression> right)
		: BinaryExpr(std::move(left), std::move(right)) {}

	fraction<int64_t> eval(const int *assignment) const override
		{ return left->eval(assignment) * right->eval(assignment); }
};

class QuotientExpr : public BinaryExpr
{
public:
	QuotientExpr(std::unique_ptr<Expression> left,
	             std::unique_ptr<Expression> right)
		: BinaryExpr(std::move(left), std::move(right)) {}

	fraction<int64_t> eval(const int *assignment) const override
		{ return left->eval(assignment) / right->eval(assignment); }
};

class WordExpr : public Expression
{
public:
	WordExpr(const char *begin, const char *end,
	         const std::map<char, int> &letterToIndex, int radix)
		: word(new int[std::distance(begin, end) + 1]), radix(radix)
	{
		size_t len = std::distance(begin, end);
		for (size_t i = 0; i < len; ++i)
			word[i] = letterToIndex.at(begin[(len-1) - i]);
		word[len] = -1;
	}

	fraction<int64_t> eval(const int *assignment) const override
	{
		int64_t res = 0, val = 1;
		for (size_t i = 0; word[i] >= 0; ++i) {
			res += assignment[word[i]] * val;
			val *= radix;
		}
		return fraction<int64_t>(res);
	}

private:
	std::unique_ptr<int[]> word;
	int radix;
};

class NumberExpr : public Expression
{
public:
	NumberExpr(const char* begin, const char* end, int radix) : value(0)
	{
		for (const char *cur = begin; cur != end; ++cur) {
			value *= radix;
			value += *cur - '0';
		}
	}

	fraction<int64_t> eval(const int*) const override
	{
		return fraction<int64_t>(value);
	}

private:
	int64_t value;
};

// END Implementation of Expressions

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

std::unique_ptr<Expression> ExpressionParser::parse(const char *expr)
{
	return parse(expr, expr + strlen(expr));
}

std::unique_ptr<Expression> ExpressionParser::parse(
	const char *begin, const char *end)
{
	NodeType type = NodeType::LEAF;
	const char *split;
	int priority = std::numeric_limits<int>::max();

	// TODO: process parantheses
	// LOW: skip whitespace (well, maybe)
	for (const char *cur = begin; cur != end; ++cur)
		for (size_t op = 0; op < (sizeof(ParseTable)/sizeof(ExprType)); ++op)
			if (*cur == ParseTable[op].op && priority >= ParseTable[op].priority) {
				type = ParseTable[op].type;
				split = cur;
				priority = ParseTable[op].priority;
			}

	// split expression and process parts recursively
	if (type != NodeType::LEAF) {
		auto left = parse(begin, split);
		auto right = parse(split+1, end);
		switch (type) {
			case NodeType::EQUAL:
				return make_unique<EqualsExpr>(std::move(left), std::move(right));
			case NodeType::PLUS:
				return make_unique<SumExpr>(std::move(left), std::move(right));
			case NodeType::MINUS:
				return make_unique<DifferenceExpr>(std::move(left), std::move(right));
			case NodeType::MULTIPLY:
				return make_unique<ProductExpr>(std::move(left), std::move(right));
			case NodeType::DIVIDE:
				return make_unique<QuotientExpr>(std::move(left), std::move(right));
			default:
				throw std::runtime_error("Unreachable code location");
		}
	}
	else {
		// Is it a word or number?
		const char *cur;
		for (cur = begin; cur != end; ++cur)
			if (*cur >= '0' && *cur <= '9')
				break;
		if (cur == end)
			return make_unique<WordExpr>(begin, end, letterToIndex, radix);
		else
			return make_unique<NumberExpr>(begin, end, radix);
	}
}

// END Implementation of ExpressionParser

// BEGIN Implementation of Puzzle

Puzzle::Puzzle(const char *puzzle, int rad)
	: radix(rad), numLetters(0), indexToLetter(rad), leading(rad)
{
	// Collect letters.
	std::map<char, int> letterToIndex;
	for (const char *cur = puzzle; *cur; ++cur)
		if (*cur >= 'A' && *cur <= 'Z')
			letterToIndex[*cur];

	// Assign numbers to letters.
	for (auto &pair : letterToIndex) {
		indexToLetter[numLetters] = pair.first;
		pair.second = numLetters++;
	}

	// Make syntax tree.
	ExpressionParser parser(letterToIndex, rad);
	root = parser.parse(puzzle);

	// Leading digits aren't allowed to be zero.
	if (puzzle[0] >= 'A' && puzzle[0] <= 'Z')
		leading[letterToIndex[puzzle[0]]] = true;
	for (int i = 1; puzzle[i]; ++i)
		if (puzzle[i-1] < 'A' && puzzle[i] >= 'A' && puzzle[i] <= 'Z')
			leading[letterToIndex[puzzle[i]]] = true;
}

bool Puzzle::eval(const int *assignment) const
{
	for (int i = 0; i < radix; ++i)
		if (!assignment[i] && leading[i])
			return false;
	return root->eval(assignment) != 0;
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

		do
			if (puzzle.eval(*mapGen)) {
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
