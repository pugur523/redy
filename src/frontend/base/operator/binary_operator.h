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

  // # exponentiation
  kPower = 1,  // **

  // # multiplicative
  kMultiply = 2,  // *
  kDivide = 3,    // /
  kModulo = 4,    // %

  // # additive
  kAdd = 5,       // +
  kSubtract = 6,  // -

  // # bitwise shift
  kLeftShift = 7,   // <<
  kRightShift = 8,  // >>

  // # comparisons
  kThreeWay = 9,  // <=>

  kLessThan = 10,            // <
  kGreaterThan = 11,         // >
  kLessThanOrEqual = 12,     // <=
  kGreaterThanOrEqual = 13,  // >=

  kCompareEqual = 14,  // ==
  kNotEqual = 15,      // !=

  // # bitwise
  kBitwiseAnd = 16,  // &
  kBitwiseXor = 17,  // ^
  kBitwiseOr = 18,   // |
  kLogicalAnd = 19,  // &&
  kLogicalOr = 20,   // ||

  // # assignment
  kDeclarationAssign = 21,  // :=
  kStandardAssign = 22,     // =

  // # compound assignment
  kAddAssign = 23,         // +=
  kSubtractAssign = 24,    // -=
  kMultiplyAssign = 25,    // *=
  kDivideAssign = 26,      // /=
  kModuloAssign = 27,      // %=
  kBitwiseAndAssign = 28,  // &=
  kBitwiseOrAssign = 29,   // |=
  kBitwiseXorAssign = 30,  // ^=
  kLeftShiftAssign = 31,   // <<=
  kRightShiftAssign = 32,  // >>=
};

inline BinaryOperator token_kind_to_binary_op(TokenKind kind) {
  switch (kind) {
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

    case TokenKind::kAssign: return BinaryOperator::kDeclarationAssign;
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
  if (op == BinaryOperator::kPower) {
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
