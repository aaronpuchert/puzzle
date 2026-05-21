#ifndef PUZZLE_HPP
#define PUZZLE_HPP

#include "arena.hpp"
#include "expr.hpp"
#include "fraction.hpp"
#include <bitset>
#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <span>

namespace puzzle {
	class ExpressionParser
	{
	public:
		ExpressionParser(
			Arena &arena, const std::map<char, Letter> &letterToIndex, int radix)
			: arena(arena), letterToIndex(letterToIndex), radix(radix) {}
		Expr *parse(const char *expr);

	private:
		Expr *parse(const char *begin, const char *end);

		Arena &arena;
		const std::map<char, Letter> &letterToIndex;
		int radix;
	};

	/**
	 * Puzzle data structure
	 */
	class Puzzle {
	public:
		static constexpr int maxNumLetters = 32;

		Puzzle(const char *puzzle, int rad);
		int getRadix() const { return radix; }
		int getNumLetters() const { return numLetters; }
		std::bitset<maxNumLetters> getLeading() const { return leading; }
		char operator[](int n) const { return indexToLetter[n]; }
		const Expr *getRoot() const { return root; }

	private:
		int radix;
		int numLetters;
		char indexToLetter[maxNumLetters];
		std::bitset<maxNumLetters> leading;
		Arena arena;
		const Expr *root;
	};

	class GenericEvaluator {
	public:
		GenericEvaluator(const Puzzle &puzzle) : puzzle(puzzle) {}

		bool operator()(const int *assignment) const;

	private:
		const Puzzle &puzzle;
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
}

#endif
