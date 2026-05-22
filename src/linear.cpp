#include "expr.hpp"
#include "puzzle.hpp"
#include "util.hpp"

namespace puzzle {

LinearEvaluator::LinearEvaluator(const Puzzle &puzzle) :
	puzzle(puzzle), coeff{}, constant(0)
{
	addCoeff(puzzle.getRoot(), 1);
}

void LinearEvaluator::addCoeff(const Expr* expr, int factor)
{
	switch (expr->getKind()) {
	case Expr::Kind::Number:
		constant += factor * cast<NumberExpr>(expr)->getValue();
		return;
	case Expr::Kind::Word: {
		const WordExpr* wordExpr = cast<WordExpr>(expr);
		std::span<const Letter> word = wordExpr->getWord();
		for (unsigned i = 0; i < word.size(); ++i) {
			coeff[word[i]] += factor;
			factor *= puzzle.getRadix();
		}
		return;
	}
	case Expr::Kind::Equality: {
		const EqualityExpr* eqExpr = cast<EqualityExpr>(expr);
		addCoeff(eqExpr->getLeft(), factor);
		addCoeff(eqExpr->getRight(), -factor);
		return;
	}
	case Expr::Kind::Binary: {
		const BinaryExpr* binExpr = cast<BinaryExpr>(expr);
		switch (binExpr->getOp()) {
		case BinaryExpr::Op::Add:
			addCoeff(binExpr->getLeft(), factor);
			addCoeff(binExpr->getRight(), factor);
			return;
		case BinaryExpr::Op::Sub:
			addCoeff(binExpr->getLeft(), factor);
			addCoeff(binExpr->getRight(), -factor);
			return;
		case BinaryExpr::Op::Mul:
		case BinaryExpr::Op::Div:
			// TODO: We could support multiplication with literals.
			throw Unsupported{};
		}
		PUZZLE_UNREACHABLE;
	}
	}
	PUZZLE_UNREACHABLE;
}

bool LinearEvaluator::operator()(const int *assignment) const
{
	std::bitset<Puzzle::maxNumLetters> leading = puzzle.getLeading();
	for (int i = 0; i < puzzle.getNumLetters(); ++i)
		if (!assignment[i] && leading[i])
			return false;

	int result = constant;
	for (int i = 0; i != puzzle.getNumLetters(); ++i)
		result += coeff[i] * assignment[i];
	return result == 0;
}

} // namespace puzzle
