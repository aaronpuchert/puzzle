//------------------------------
//  PUZZLE SOLVER MAIN HEADER
//------------------------------
#include <vector>
#include <map>
#include <ostream>
#include "fraction.hpp"

namespace Puzzle {
	/**
	 * Expression type
	 */
	class Expr {
	public:
		Expr(const char *expr, int len, const std::map<char, int> &transmap, int rad);
		~Expr();
		fraction<int> Eval(const int* NumMap) const;

	private:
		// Node types
		enum class NodeType;
		NodeType type;

		// Internal parser table
		struct ExprType;
		static const ExprType ParseTable[];

		union {
			// inner nodes
			struct {
				Expr *left, *right;
			};
			// word leaf
			struct {
				int *word;
				int radix;
			};
			// number leaf
			int value;
		};
	};

	/**
	 * Puzzle data structure
	 */
	class Puzz {
	public:
		Puzz(const char *puzzle, int rad);
		~Puzz();
		bool Eval(const int *NumMap) const;
		int DomainSize() const {return num;}
		char operator[](int n) const {return lettermap[n];}

		int radix;
	private:
		int num;
		std::vector<char> lettermap;
		std::vector<bool> leading;
		Expr *root;
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
