//------------------------------
//  PUZZLE SOLVER MAIN HEADER
//------------------------------
#include <vector>
#include <map>

namespace Puzzle {
	// node types
	enum NodeType {
		EQUAL = 0,			// should occur exactly once - at root
		PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		// FACTORIAL,
		// BINOMIAL,   etc.
		// LOW: add more operators
		LEAF = 0x1000,		// dummy type
		WORD,				// "fixed-value"/number leaf
		NUMBER,				// "variable-value"/word leaf
		FORCE_DWORD	= 0x7fffffff
	};

	// LOW: exceptions...

	// expression type
	class Expr {
	protected:
		NodeType type;
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

	public:
		Expr(const char *expr, int len, const std::map<char, int> &transmap, int rad);
		~Expr();
		int Eval(const int *NumMap) const;
	};

	// puzzle data structure
	class Puzz {
	protected:
		int radix;
		int num;
		std::vector<char> lettermap;
		std::vector<bool> leading;
		Expr *root;

	public:
		Puzz(const char *puzzle, int rad);
		~Puzz();
		bool Eval(const int *NumMap) const;
		int DomainSize() const {return num;}
		char operator[](int n) const {return lettermap[n];}
	};

	// generates all injective maps
	class MapGen {
	protected:
		int n;		// codomain size
		int m;		// domain size
		int *map;	// current map

	public:
		MapGen(int DomSize, int CodSize);
		~MapGen();
		int operator [](int i) const {return map[i];}
		int *operator *() const {return map;}
		bool NextMap();
	};
}