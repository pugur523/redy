// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_BINARY_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_BINARY_OPERATOR_H_

#include <cstdint>
#include <type_traits>

#include "frontend/base/operator/operator.h"
#include "frontend/base/token/token_kind.h"

namespace base {

enum class BinaryOperator : uint8_t {
  kUnknown = 0,

  // # cast
  kAs = 1,

  // # exponentiation
  kPower = 2,  // **

  // # multiplicative
  kMultiply = 3,  // *
  kDivide = 4,    // /
  kModulo = 5,    // %

  // # additive
  kAdd = 6,       // +
  kSubtract = 7,  // -

  // # bitwise shift
  kLeftShift = 8,   // <<
  kRightShift = 9,  // >>

  // # comparisons
  kThreeWay = 10,  // <=>

  kLessThan = 11,            // <
  kGreaterThan = 12,         // >
  kLessThanOrEqual = 13,     // <=
  kGreaterThanOrEqual = 14,  // >=

  kCompareEqual = 15,  // ==
  kNotEqual = 16,      // !=

  // # bitwise
  kBitwiseAnd = 17,  // &
  kBitwiseXor = 18,  // ^
  kBitwiseOr = 19,   // |
  kLogicalAnd = 20,  // &&
  kLogicalOr = 21,   // ||

  // # assignment
  kDeclarationAssign = 22,  // :=
  kStandardAssign = 23,     // =

  // # compound assignment
  kAddAssign = 24,         // +=
  kSubtractAssign = 25,    // -=
  kMultiplyAssign = 26,    // *=
  kDivideAssign = 27,      // /=
  kModuloAssign = 28,      // %=
  kBitwiseAndAssign = 29,  // &=
  kBitwiseOrAssign = 30,   // |=
  kBitwiseXorAssign = 31,  // ^=
  kLeftShiftAssign = 32,   // <<=
  kRightShiftAssign = 33,  // >>=
};

inline BinaryOperator token_kind_to_binary_op(TokenKind kind) {
  switch (kind) {
    case TokenKind::kAs: return BinaryOperator::kAs;
    case TokenKind::kStarStar: return BinaryOperator::kPower;

    case TokenKind::kStar: return BinaryOperator::kMultiply;
    case TokenKind::kSlash: return BinaryOperator::kDivide;
    case TokenKind::kPercent: return BinaryOperator::kModulo;

    case TokenKind::kPlus: return BinaryOperator::kAdd;
    case TokenKind::kMinus: return BinaryOperator::kSubtract;

    case TokenKind::kLtLt: return BinaryOperator::kLeftShift;
    case TokenKind::kGtGt: return BinaryOperator::kRightShift;

    case TokenKind::kThreeWay: return BinaryOperator::kThreeWay;
    case TokenKind::kLt: return BinaryOperator::kLessThan;
    case TokenKind::kGt: return BinaryOperator::kGreaterThan;
    case TokenKind::kLe: return BinaryOperator::kLessThanOrEqual;
    case TokenKind::kGe: return BinaryOperator::kGreaterThanOrEqual;
    case TokenKind::kEqEq: return BinaryOperator::kCompareEqual;
    case TokenKind::kNotEqual: return BinaryOperator::kNotEqual;

    case TokenKind::kAnd: return BinaryOperator::kBitwiseAnd;
    case TokenKind::kCaret: return BinaryOperator::kBitwiseXor;
    case TokenKind::kPipe: return BinaryOperator::kBitwiseOr;
    case TokenKind::kAndAnd: return BinaryOperator::kLogicalAnd;
    case TokenKind::kPipePipe: return BinaryOperator::kLogicalOr;

    case TokenKind::kColonEqual: return BinaryOperator::kDeclarationAssign;
    case TokenKind::kEqual: return BinaryOperator::kStandardAssign;

    case TokenKind::kPlusEq: return BinaryOperator::kAddAssign;
    case TokenKind::kMinusEq: return BinaryOperator::kSubtractAssign;
    case TokenKind::kStarEq: return BinaryOperator::kMultiplyAssign;
    case TokenKind::kSlashEq: return BinaryOperator::kDivideAssign;
    case TokenKind::kPercentEq: return BinaryOperator::kModuloAssign;
    case TokenKind::kAndEq: return BinaryOperator::kBitwiseAndAssign;
    case TokenKind::kPipeEq: return BinaryOperator::kBitwiseOrAssign;
    case TokenKind::kCaretEq: return BinaryOperator::kBitwiseXorAssign;
    case TokenKind::kLtLtEq: return BinaryOperator::kLeftShiftAssign;
    case TokenKind::kGtGtEq: return BinaryOperator::kRightShiftAssign;
    default: return BinaryOperator::kUnknown;
  }
}

inline bool token_kind_is_binary_operator(TokenKind kind) {
  return token_kind_to_binary_op(kind) != BinaryOperator::kUnknown;
}

inline const char* binary_op_to_string(BinaryOperator op) {
  static constexpr const char* kNames[] = {
      "unknown",
      "as",
      "power",
      "multiply",
      "divide",
      "modulo",
      "add",
      "subtract",
      "left shift",
      "right shift",
      "three way",
      "less than",
      "greater than",
      "less than or equal",
      "greater than or equal",
      "compare equal",
      "not equal",
      "bitwise and",
      "bitwise xor",
      "bitwise or",
      "and",
      "or",
      "init assign equal",
      "normal assign equal",
      "add assign",
      "subtract assign",
      "multiply assign",
      "divide assign",
      "modulo assign",
      "bitwise and assign",
      "bitwise or assign",
      "bitwise xor assign",
      "left shift assign",
      "right shift assign",
      "invalid",
  };
  const auto idx = static_cast<std::size_t>(op);
  DCHECK_LT(idx, std::size(kNames));
  return kNames[idx];
}

inline OperatorPrecedence binary_op_to_precedence(BinaryOperator op) {
  if (op == BinaryOperator::kAs) {
    return OperatorPrecedence::kCast;
  } else if (op == BinaryOperator::kPower) {
    return OperatorPrecedence::kExponentiation;
  } else if (op >= BinaryOperator::kMultiply && op <= BinaryOperator::kModulo) {
    return OperatorPrecedence::kMultiplicative;
  } else if (op >= BinaryOperator::kAdd && op <= BinaryOperator::kSubtract) {
    return OperatorPrecedence::kAdditive;
  } else if (op >= BinaryOperator::kLeftShift &&
             op <= BinaryOperator::kRightShift) {
    return OperatorPrecedence::kBitwiseShift;
  } else if (op >= BinaryOperator::kThreeWay &&
             op <= BinaryOperator::kThreeWay) {
    return OperatorPrecedence::kThreeWayComparison;
  } else if (op >= BinaryOperator::kLessThan &&
             op <= BinaryOperator::kGreaterThanOrEqual) {
    return OperatorPrecedence::kRelationalComparison;
  } else if (op >= BinaryOperator::kCompareEqual &&
             op <= BinaryOperator::kNotEqual) {
    return OperatorPrecedence::kEqualityComparison;
  } else if (op == BinaryOperator::kBitwiseAnd) {
    return OperatorPrecedence::kBitwiseAnd;
  } else if (op == BinaryOperator::kBitwiseXor) {
    return OperatorPrecedence::kBitwiseXor;
  } else if (op == BinaryOperator::kBitwiseOr) {
    return OperatorPrecedence::kBitwiseOr;
  } else if (op == BinaryOperator::kLogicalAnd) {
    return OperatorPrecedence::kLogicalAnd;
  } else if (op == BinaryOperator::kLogicalOr) {
    return OperatorPrecedence::kLogicalOr;
  } else if (op >= BinaryOperator::kDeclarationAssign &&
             op <= BinaryOperator::kStandardAssign) {
    return OperatorPrecedence::kAssignment;
  } else if (op >= BinaryOperator::kAddAssign &&
             op <= BinaryOperator::kModuloAssign) {
    return OperatorPrecedence::kCompoundAssignment;
  } else if (op >= BinaryOperator::kBitwiseAndAssign &&
             op <= BinaryOperator::kRightShiftAssign) {
    return OperatorPrecedence::kBitwiseCompoundAssignment;
  }

  DCHECK(false);
  return OperatorPrecedence::kUnknown;
}

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_BINARY_OPERATOR_H_
