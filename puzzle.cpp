#include "puzzle.h"
#include <utility>
#include <iterator>
#include <limits>

struct ExprType {
	char op;
	Puzzle::NodeType type;
	int priority;
};

const int ParseTableEntries = 5; 
const ExprType ParseTable[] = {
	{'=', Puzzle::EQUAL, 0},
	{'+', Puzzle::PLUS, 1},
	{'-', Puzzle::MINUS, 1},
	{'*', Puzzle::MULTIPLY, 2},
	{'/', Puzzle::DIVIDE, 2}
};

//------------------------------
// IMPLEMENTATION OF EXPRESSIONS
//------------------------------

Puzzle::Expr::Expr(const char* expr, int len, const std::map<char, int> &transmap, int rad)
{
	type = LEAF;
	int split;
	int priority = std::numeric_limits<int>::max();

	// TODO: process parantheses
	// LOW: skip whitespace (well, maybe)
	for (int i=0; i<len; ++i)
		for (int op=0; op<ParseTableEntries; ++op)			// LOW: what about a std::map?
			if (expr[i]	== ParseTable[op].op && priority >= ParseTable[op].priority) {
				type = ParseTable[op].type;
				split = i;
				priority = ParseTable[op].priority;
			}

	// split expression and process parts recursively
	if (type != LEAF) {
		left = new Expr(expr, split, transmap, rad);
		right = new Expr(expr+split+1, len-split-1, transmap, rad);
	}
	else {
		// word or number?
		for (i=0; i<len; ++i)
			if (expr[i] >= '0' && expr[i] <= '9')
				break;
		if (i == len) {	 // word
			type = WORD;
			word = new int[len+1];
			for (i=0; i<len; ++i)
				word[i] = transmap.find(expr[(len-1)-i])->second;
			word[len] = -1;
			radix = rad;
		}
		else {			 // number
			type = NUMBER;
			const char *end;
			end = expr+len;
			value = strtol(expr, (char **)&end, rad);	// ooh, dirty! (doesn't matter here)
			// LOW: catch errors?
		}
	}
}

Puzzle::Expr::~Expr()
{
	if (type & LEAF) {		// leaf
		if (type == WORD)
			delete word;
	}
	else {					// inner node
		delete left;
		delete right;
	}
}

int Puzzle::Expr::Eval(const int *NumMap) const
{
	int res=0, val=1;

	switch (type) {
		case EQUAL:
			return (int)(left->Eval(NumMap) == right->Eval(NumMap));
		case PLUS:
			return left->Eval(NumMap) + right->Eval(NumMap);
		case MINUS:
			return left->Eval(NumMap) - right->Eval(NumMap);
		case MULTIPLY:
			return left->Eval(NumMap) * right->Eval(NumMap);
		case DIVIDE:
			res = left->Eval(NumMap);
			val = right->Eval(NumMap);
			if (res%val == 0)
				return res / val;
			else
				return -1;				 //	TODO: find better solution (fraction type?)
		case WORD:
			for (int i=0; word[i]>=0; ++i) {
				res += NumMap[word[i]]*val;
				val *= radix;
			}
			return res;
		case NUMBER:
			return value;
	}
}

//------------------------------
//  IMPLEMENTATION OF PUZZLES
//------------------------------

Puzzle::Puzz::Puzz(const char *puzzle, int rad) : lettermap(rad), leading(rad), radix(rad)
{
	// fill Map
	std::map<char, int> Map;
	for (int i=0; puzzle[i]; ++i)
		if (puzzle[i]>='A' && puzzle[i]<='Z')
			Map.insert(std::pair<char, int>(puzzle[i], -1));

	// assign numbers to letters
	num = 0;
	for (std::map<char, int>::iterator it = Map.begin(); it != Map.end(); ++it, ++num) {
		lettermap[num] = it->first;
		it->second = num;
	}

	// make	syntax tree
	root = new Expr(puzzle, (int)strlen(puzzle), Map, rad);

	// leading digits aren't allowed to	be zero
	if (puzzle[0] >= 'A' && puzzle[0] <= 'Z')
		leading[Map[puzzle[0]]] = true;
	for (int i=1; puzzle[i]; ++i)
		if (puzzle[i-1] < 'A' && puzzle[i] >= 'A' && puzzle[i] <= 'Z')
			leading[Map[puzzle[i]]] = true;
}

Puzzle::Puzz::~Puzz()
{
	delete root;
}

bool Puzzle::Puzz::Eval(const int *NumMap) const
{
	for (int i=0; i<radix; ++i)
		if (!NumMap[i] && leading[i])
			return false;
	return (bool)(root->Eval(NumMap));
}

//------------------------------
// PERMUTATION GENERATOR IMPLEMENTATION
//------------------------------
Puzzle::MapGen::MapGen(int DomSize, int CodSize) : n(CodSize), m(DomSize)
{
	if (CodSize < DomSize)
		throw("There are no injektive maps if the codomain is smaller than the domain.");
	map = new int[DomSize];

	// M0
	for (int i=0; i<DomSize; ++i)
		map[i] = i;
}

Puzzle::MapGen::~MapGen()
{
	delete map;
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
//     else {++a_j; while (j<m)	a_{++j}	<- a_{j-1}+1; goto M1}
bool Puzzle::MapGen::NextMap()
{
	int j, l, k, Temp;

	// M2
	j = m-2;		// "j <- m-1"
	while (j>=0 && (map[j] >= map[j+1])) --j;
	if (j >= 0) {
		// M3
		l = m-1;	// "l <- m"
		while (map[j] >= map[l]) --l;
		Temp = map[j]; map[j] = map[l]; map[l] = Temp;
	}

	// M4
	k = j+1;		// "k <- j+1"
	l = m-1;		// "l <- m"
	while (k<l) {
		Temp = map[k]; map[k] = map[l]; map[l] = Temp;
		++k; --l;
	}
	
	if (j < 0) {
		// M5
		j = m-1;	// "j <- m"
		int diff = n-m;
		while (j>=0 && (map[j] == j+diff)) --j;
		if (j<0) return false;
		else {
			l = map[j];
			while (j<m) map[j++] = ++l;
		}
	}

	return true;
}