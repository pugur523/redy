// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_OPERATOR_H_

#include <cstdint>

#include "frontend/base/token/token_kind.h"

namespace base {

inline bool is_operator(TokenKind kind) {
  return kind >= TokenKind::kOperatorsBegin && kind <= TokenKind::kOperatorsEnd;
}

// lower value means it is high priority
enum class OperatorPrecedence : uint8_t {
  kUnknown = 0,
  kUnary = 1,   // -x, !x, x++, x--,
  kBinary = 2,  // x ** y, x / y, x + y, x << y, x == y, x && y, x := y, x += y,
};

// enum class UnaryOperatorPrecedence : uint8_t {
// };

enum class BinaryOperatorPrecedence : uint8_t {
  kUnknown = 0,
  kExponentiation = 1,      // **
  kMultiplicative = 2,      // *, /, %
  kAdditive = 3,            // +, -
  kBitwiseShift = 4,        // <<, >>
  kComparison = 5,          // ==, !=, <, <=, >, >=
  kBitwise = 6,             // &, |, ^, &&, ||
  kAssignment = 7,          // :=, =
  kCompoundAssignment = 8,  // +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
};

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_OPERATOR_H_
