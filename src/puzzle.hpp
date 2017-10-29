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
		ExpressionParser(const std::map<char, int> &transmap, int radix)
			: transmap(transmap), radix(radix) {}
		std::unique_ptr<Expression> parse(const char *expr);

	private:
		std::unique_ptr<Expression> parse(const char *begin, const char *end);

		const std::map<char, int> &transmap;
		int radix;
	};
	std::unique_ptr<Expression> parse(
		const char* expr, size_t len,
		const std::map<char, int> &transmap, int radix);

	/**
	 * Puzzle data structure
	 */
	class Puzz {
	public:
		Puzz(const char *puzzle, int rad);
		bool eval(const int *assignment) const;
		int DomainSize() const {return num;}
		char operator[](int n) const {return lettermap[n];}

		int radix;
	private:
		int num;
		std::vector<char> lettermap;
		std::vector<bool> leading;
		std::unique_ptr<Expression> root;
	};

	/**
	 * Generates all injective maps
	 */
	class MapGen {
	public:
		MapGen(int DomSize, int CodSize);
		~MapGen();
		int operator [](int i) const {return map[i];}
		int *operator *() const {return map;}
		bool NextMap();

	private:
		int n;      // codomain size
		int m;      // domain size
		int *map;   // current map
	};

	/**
	 * Puzzle solver
	 */
	class PuzzleSolver {
	public:
		PuzzleSolver(const Puzz &puzz);
		int print_solutions(std::ostream& out, bool terminal);

	private:
		const Puzz &puzz;
	};

	// LOW: exceptions...
}

#endif
