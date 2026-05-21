#include "expr.hpp"
#include "puzzle.hpp"
#include "util.hpp"

namespace puzzle {

static fraction<int64_t> eval(
	const Expr* expr, int radix, const int *assignment)
{
	switch (expr->getKind()) {
	case Expr::Kind::Number:
		return fraction<int64_t>(cast<NumberExpr>(expr)->getValue());
	case Expr::Kind::Word: {
		const WordExpr* wordExpr = cast<WordExpr>(expr);
		int64_t res = 0, val = 1;
		for (Letter letter : wordExpr->getWord()) {
			res += assignment[letter] * val;
			val *= radix;
		}
		return fraction<int64_t>(res);
	}
	case Expr::Kind::Equality: {
		const EqualityExpr* eqExpr = cast<EqualityExpr>(expr);
		return eval(eqExpr->getLeft(), radix, assignment)
			== eval(eqExpr->getRight(), radix, assignment);
	}
	case Expr::Kind::Binary: {
		const BinaryExpr* binExpr = cast<BinaryExpr>(expr);
		switch (binExpr->getOp()) {
		case BinaryExpr::Op::Add:
			return eval(binExpr->getLeft(), radix, assignment)
				+ eval(binExpr->getRight(), radix, assignment);
		case BinaryExpr::Op::Sub:
			return eval(binExpr->getLeft(), radix, assignment)
				- eval(binExpr->getRight(), radix, assignment);
		case BinaryExpr::Op::Mul:
			return eval(binExpr->getLeft(), radix, assignment)
				* eval(binExpr->getRight(), radix, assignment);
		case BinaryExpr::Op::Div:
			return eval(binExpr->getLeft(), radix, assignment)
				/ eval(binExpr->getRight(), radix, assignment);
		}
		PUZZLE_UNREACHABLE;
	}
	}
	PUZZLE_UNREACHABLE;
}

bool GenericEvaluator::operator()(const int *assignment) const
{
	auto leading = puzzle.getLeading();
	for (int i = 0; i < puzzle.getNumLetters(); ++i)
		if (!assignment[i] && leading[i])
			return false;
	return eval(puzzle.getRoot(), puzzle.getRadix(), assignment) != 0;
}

} // namespace puzzle
