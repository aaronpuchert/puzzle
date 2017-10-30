#ifndef PUZZLE_HPP
#define PUZZLE_HPP

//------------------------------
//  PUZZLE SOLVER MAIN HEADER
//------------------------------
#include <vector>
#include <map>
#include <memory>
#include <ostream>
#include "fraction.hpp"

namespace Puzzle {
	/**
	 * Expression type
	 */
	class Expression
	{
	public:
		virtual ~Expression();
		virtual fraction<int64_t> eval(const int *assignment) const = 0;
	};

	class ExpressionParser
	{
	public:
		ExpressionParser(const std::map<char, int> &letterToIndex, int radix)
			: letterToIndex(letterToIndex), radix(radix) {}
		std::unique_ptr<Expression> parse(const char *expr);

	private:
		std::unique_ptr<Expression> parse(const char *begin, const char *end);

		const std::map<char, int> &letterToIndex;
		int radix;
	};
	std::unique_ptr<Expression> parse(
		const char* expr, size_t len,
		const std::map<char, int> &letterToIndex, int radix);

	/**
	 * Puzzle data structure
	 */
	class Puzzle {
	public:
		Puzzle(const char *puzzle, int rad);
		bool eval(const int *assignment) const;
		int getRadix() const { return radix; }
		int getNumLetters() const { return numLetters; }
		char operator[](int n) const { return indexToLetter[n]; }

	private:
		int radix;
		int numLetters;
		std::vector<char> indexToLetter;
		std::vector<bool> leading;
		std::unique_ptr<Expression> root;
	};

	/**
	 * Generates all injective maps
	 */
	class MapGen {
	public:
		MapGen(int domainSize, int codomainSize);
		~MapGen();
		int operator [](int i) const { return map[i]; }
		int *operator *() const { return map.get(); }
		bool nextMap();

	private:
		int n;      ///< Codomain size
		int m;      ///< Domain size
		std::unique_ptr<int[]> map;
	};

	/**
	 * Puzzle solver
	 */
	class PuzzleSolver {
	public:
		PuzzleSolver(const Puzzle &puzz);
		int print_solutions(std::ostream& out, bool terminal);

	private:
		const Puzzle &puzzle;
	};

	// LOW: exceptions...
}

#endif
