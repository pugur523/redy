// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_BINARY_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_BINARY_OPERATOR_H_

#include <cstdint>

#include "frontend/base/base_export.h"
#include "frontend/base/token/token.h"
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

  // # comparison
  kLessThan = 9,             // <
  kGreaterThan = 10,         // >
  kLessThanOrEqual = 11,     // <=
  kGreaterThanOrEqual = 12,  // >=
  kEqualEqual = 13,          // ==
  kNotEqual = 14,            // !=

  // # bitwise
  kBitwiseAnd = 15,  // &
  kBitwiseXor = 16,  // ^
  kBitwiseOr = 17,   // |
  kAnd = 18,         // &&
  kOr = 19,          // ||

  // # assignment
  kAssign = 20,  // :=
  kEqual = 21,   // =

  // # compound assignment
  kAddAssign = 22,         // +=
  kSubtractAssign = 23,    // -=
  kMultiplyAssign = 24,    // *=
  kDivideAssign = 25,      // /=
  kModuloAssign = 26,      // %=
  kBitwiseAndAssign = 27,  // &=
  kBitwiseOrAssign = 28,   // |=
  kBitwiseXorAssign = 29,  // ^=
  kLeftShiftAssign = 30,   // <<=
  kRightShiftAssign = 31,  // >>=
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

    case TokenKind::kLt: return BinaryOperator::kLessThan;
    case TokenKind::kGt: return BinaryOperator::kGreaterThan;
    case TokenKind::kLe: return BinaryOperator::kLessThanOrEqual;
    case TokenKind::kGe: return BinaryOperator::kGreaterThanOrEqual;
    case TokenKind::kEqEq: return BinaryOperator::kEqualEqual;
    case TokenKind::kNotEqual: return BinaryOperator::kNotEqual;

    case TokenKind::kAnd: return BinaryOperator::kBitwiseAnd;
    case TokenKind::kCaret: return BinaryOperator::kBitwiseXor;
    case TokenKind::kPipe: return BinaryOperator::kBitwiseOr;
    case TokenKind::kAndAnd: return BinaryOperator::kAnd;
    case TokenKind::kPipePipe: return BinaryOperator::kOr;

    case TokenKind::kAssign: return BinaryOperator::kAssign;
    case TokenKind::kEqual: return BinaryOperator::kEqual;

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
  switch (op) {
    case BinaryOperator::kUnknown: return "unknown";

    case BinaryOperator::kPower: return "power";

    case BinaryOperator::kMultiply: return "multiply";
    case BinaryOperator::kDivide: return "devide";
    case BinaryOperator::kModulo: return "modulo";

    case BinaryOperator::kAdd: return "add";
    case BinaryOperator::kSubtract: return "subtract";

    case BinaryOperator::kLeftShift: return "left shift";
    case BinaryOperator::kRightShift: return "right shift";

    case BinaryOperator::kLessThan: return "less than";
    case BinaryOperator::kGreaterThan: return "greater than";
    case BinaryOperator::kLessThanOrEqual: return "less than or equal";
    case BinaryOperator::kGreaterThanOrEqual: return "greater than or equal";
    case BinaryOperator::kEqualEqual: return "equal equal";
    case BinaryOperator::kNotEqual: return "not equal";

    case BinaryOperator::kBitwiseAnd: return "bitwise and";
    case BinaryOperator::kBitwiseXor: return "bitwise xor";
    case BinaryOperator::kBitwiseOr: return "bitwise or";
    case BinaryOperator::kAnd: return "and";
    case BinaryOperator::kOr: return "or";

    case BinaryOperator::kAssign: return "assign";
    case BinaryOperator::kEqual: return "equal";

    case BinaryOperator::kAddAssign: return "add assign";
    case BinaryOperator::kSubtractAssign: return "subtract assign";
    case BinaryOperator::kMultiplyAssign: return "multiply assign";
    case BinaryOperator::kDivideAssign: return "divide assign";
    case BinaryOperator::kModuloAssign: return "modulo assign";
    case BinaryOperator::kBitwiseAndAssign: return "bitwise and assign";
    case BinaryOperator::kBitwiseOrAssign: return "bitwise or assign";
    case BinaryOperator::kBitwiseXorAssign: return "bitwise xor assign";
    case BinaryOperator::kLeftShiftAssign: return "left shift assign";
    case BinaryOperator::kRightShiftAssign: return "right shift assign";
    default: DCHECK(false); return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_BINARY_OPERATOR_H_
