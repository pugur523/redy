// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_OPERATOR_H_

#include <cstdint>

#include "core/check.h"

namespace base {

// lower value means it has high priority
// evaluate unary operations first, then binary operations
// see README.md for detail.
enum class OperatorPrecedence : uint8_t {
  kUnknown = 0,
  kPostUnary = 1,                   // a++ a--
  kPreUnary = 2,                    // ++a --a !a ^a ~a
  kCast = 3,                        // a as b
  kExponentiation = 4,              // a ** b
  kMultiplicative = 5,              // a * b a / b a % b
  kAdditive = 6,                    // a + b a - b
  kBitwiseShift = 7,                // a << b a >> b
  kThreeWayComparison = 8,          // a <=> b
  kRelationalComparison = 9,        // a < b a <= b a > b a >= b
  kEqualityComparison = 10,         // a == b a != b
  kBitwiseAnd = 11,                 // a & b
  kBitwiseXor = 12,                 // a ^ b
  kBitwiseOr = 13,                  // a \| b
  kLogicalAnd = 14,                 // a && b
  kLogicalOr = 15,                  // a \|\| b
  kAssignment = 16,                 // a = b a := b
  kCompoundAssignment = 16,         // a += b a -= b a *= b a /= b a %= b
  kBitwiseCompoundAssignment = 16,  // a &= b a ^= b a \|= b a <<= b a >>= b

  kLowest = kBitwiseCompoundAssignment,
};

enum class OperatorAssociativity : uint8_t {
  kUnknown = 0,
  kLeftToRight = 1,
  kRightToLeft = 2,
};

inline OperatorAssociativity operator_precedence_to_associativity(
    OperatorPrecedence precedence) {
  switch (precedence) {
    case OperatorPrecedence::kUnknown: return OperatorAssociativity::kUnknown;
    case OperatorPrecedence::kPreUnary:
    case OperatorPrecedence::kExponentiation:
    case OperatorPrecedence::kAssignment:
      return OperatorAssociativity::kRightToLeft;
    default: return OperatorAssociativity::kLeftToRight;
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_OPERATOR_H_
