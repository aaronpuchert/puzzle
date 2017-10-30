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
	EQUAL = 0,          // should occur exactly once - at root
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	// FACTORIAL,
	// BINOMIAL,   etc.
	WORD = 0x1000,      // "fixed-value"/number leaf
	NUMBER              // "variable-value"/word leaf
};

struct ExprType {
	char op;
	NodeType type;
	int priority;
};

static const ExprType ParseTable[] = {
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
	NodeType type = NodeType::WORD;
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
	if (type != NodeType::WORD) {
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

//------------------------------
//  IMPLEMENTATION OF PUZZLES
//------------------------------

Puzzle::Puzzle(const char *puzzle, int rad) : radix(rad), indexToLetter(rad), leading(rad)
{
	// fill Map
	std::map<char, int> letterToIndex;
	for (int i = 0; puzzle[i]; ++i)
		if (puzzle[i] >= 'A' && puzzle[i] <= 'Z')   // what about nondecimal digits?
			letterToIndex.insert(std::pair<char, int>(puzzle[i], -1));

	// assign numbers to letters
	numLetters = 0;
	for (auto it = letterToIndex.begin(); it != letterToIndex.end(); ++it, ++numLetters) {
		indexToLetter[numLetters] = it->first;
		it->second = numLetters;
	}

	// make syntax tree
	ExpressionParser parser(letterToIndex, rad);
	root = parser.parse(puzzle);

	// leading digits aren't allowed to be zero
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

//------------------------------
// PERMUTATION GENERATOR IMPLEMENTATION
//------------------------------
MapGen::MapGen(int domainSize, int codomainSize)
	: n(codomainSize), m(domainSize)
{
	if (codomainSize < domainSize)
		throw std::domain_error("There are no injective maps if the codomain is smaller than the domain.");
	map = new int[domainSize];

	// M0
	for (int i = 0; i < domainSize; ++i)
		map[i] = i;
}

MapGen::~MapGen()
{
	delete[] map;
}

//---------------------------  ALGORITHM DESCRIPTION  -------------------------
// (The following algorithm is inspired by Donald E. Knuth: The Art of Computer Programming, Vol. 4, Fasc. 2, Algorithm L; but slightly modified)
// Algorithm M: Visit all injective maps f from {1, ..., m} to {1, ..., n}, given m<n.
// Idea: Run through all m-subsets of {1, ..., n} and visit all permutations of these subsets for each one.
//   Running through all m-subsets is done by a "combination-enumeration" algorithm which outputs all combinations in a canonical order.
//   Running through all permutations of these subsets is done by Alg. L.
// M0. start with map (a_1, ..., a_m) = (1, ..., m).
// M1. visit map (a_1, ..., a_m)
// M2. [Find j] j <- m-1; while (a_j >= a_{j+1}) --j;
//     if (j=0) goto M4;
// M3. [Next a_j] l <- n; while (a_j >= a_l) --l; a_j <-> a_l;
// M4. [Reverse a_{j+1}, ..., a_m] k<-j+1; l<-m; while (k<l) {a_k <-> a_l, k++, l--} if (j>0) goto M1.
// M5. [Next combination] j <- m; while (a_j == j+(n-m)) --j;
//     if (j=0) finished;
//     else {++a_j; while (j<m) a_{++j} <- a_{j-1}+1; goto M1}
bool MapGen::nextMap()
{
	int j, l, k;

	// M2
	j = m - 2;      // "j <- m-1"
	while (j >= 0 && (map[j] >= map[j+1]))
		--j;
	if (j >= 0) {
		// M3
		l = m - 1;  // "l <- m"
		while (map[j] >= map[l])
			--l;
		std::swap(map[j], map[l]);
	}

	// M4
	k = j + 1;      // "k <- j+1"
	l = m - 1;      // "l <- m"
	while (k < l) {
		std::swap(map[k], map[l]);
		++k; --l;
	}

	if (j < 0) {
		// M5
		j = m - 1;  // "j <- m"
		int diff = n-m;
		while (j >= 0 && (map[j] == j+diff))
			--j;
		if (j < 0)
			return false;
		else {
			l = map[j];
			while (j < m)
				map[j++] = ++l;
		}
	}

	return true;
}

//------------------------------
//        PUZZLE SOLVER
//------------------------------
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
	catch (const std::domain_error &err) {
		out << "This alphametic has too many letters.\n\n";
	}

	return numSolutions;
}

}
