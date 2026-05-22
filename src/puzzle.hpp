#ifndef PUZZLE_HPP
#define PUZZLE_HPP

#include "fraction.hpp"
#include <bitset>
#include <cstdint>
#include <map>
#include <memory>
#include <ostream>

namespace puzzle {
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

	/**
	 * Puzzle data structure
	 */
	class Puzzle {
	public:
		static constexpr int maxNumLetters = 32;

		Puzzle(const char *puzzle, int rad);
		bool eval(const int *assignment) const;
		int getRadix() const { return radix; }
		int getNumLetters() const { return numLetters; }
		char operator[](int n) const { return indexToLetter[n]; }

	private:
		int radix;
		int numLetters;
		char indexToLetter[maxNumLetters];
		std::bitset<maxNumLetters> leading;
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
}

#endif
