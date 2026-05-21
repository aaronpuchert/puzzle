#ifndef PUZZLE_EXPR
#define PUZZLE_EXPR

#include "arena.hpp"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <span>
#include <type_traits>

namespace puzzle {

using Letter = unsigned char;

class Expr {
public:
	enum class Kind {
		Number,
		Word,
		Equality,
		Binary,
	};

public:
	Expr(Kind kind) : kind(kind) {}

	Kind getKind() const { return kind; }

private:
	const Kind kind;
};

class NumberExpr : public Expr {
public:
	static NumberExpr *create(Arena &arena, int value)
	{
		static_assert(std::is_trivially_destructible_v<NumberExpr>);
		static_assert(alignof(NumberExpr) <= Arena::alignment);
		return new(arena) NumberExpr(value);
	}

	int getValue() const { return value; }

	static bool classof(const Expr *E) { return E->getKind() == Kind::Number; }

private:
	NumberExpr(int value) : Expr(Kind::Number), value(value) {}

	int value;
};

class WordExpr : public Expr {
public:
	static constexpr unsigned maxSize = 16;

	static WordExpr *create(Arena &arena, const Letter *letters, unsigned size)
	{
		static_assert(std::is_trivially_destructible_v<WordExpr>);
		static_assert(alignof(WordExpr) <= Arena::alignment);
		assert(size <= maxSize);
		return new(arena) WordExpr(letters, size);
	}

	std::span<const Letter> getWord() const
	{
		return std::span<const Letter>(word, size);
	}

	static bool classof(const Expr *E) { return E->getKind() == Kind::Word; }

private:
	WordExpr(const Letter *letters, unsigned size)
		: Expr(Kind::Word), size(size)
	{
		std::memcpy(word, letters, size);
	}

	unsigned size;
	Letter word[maxSize];
};

class EqualityExpr : public Expr {
public:
	static EqualityExpr *create(
		Arena &arena, const Expr *left, const Expr *right)
	{
		static_assert(std::is_trivially_destructible_v<EqualityExpr>);
		static_assert(alignof(EqualityExpr) <= Arena::alignment);
		return new(arena) EqualityExpr(left, right);
	}

	const Expr* getLeft() const { return left; }
	const Expr* getRight() const { return right; }

	static bool classof(const Expr *E) { return E->getKind() == Kind::Equality; }

private:
	EqualityExpr(const Expr *left, const Expr *right)
		: Expr(Kind::Equality), left(left), right(right) {}

	const Expr *left, *right;
};

class BinaryExpr : public Expr {
public:
	enum class Op {
		Add,
		Sub,
		Mul,
		Div,
	};

	static BinaryExpr *create(
		Arena &arena, Op op, const Expr *left, const Expr *right)
	{
		static_assert(std::is_trivially_destructible_v<BinaryExpr>);
		static_assert(alignof(BinaryExpr) <= Arena::alignment);
		return new(arena) BinaryExpr(op, left, right);
	}

	Op getOp() const { return op; }
	const Expr* getLeft() const { return left; }
	const Expr* getRight() const { return right; }

	static bool classof(const Expr *E) { return E->getKind() == Kind::Binary; }

private:
	BinaryExpr(Op op, const Expr *left, const Expr *right)
		: Expr(Kind::Binary), op(op), left(left), right(right) {}

	Op op;
	const Expr *left, *right;
};

} // namespace puzzle

#endif
