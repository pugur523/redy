// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_OPERATOR_H_

#include <cstdint>
#include <limits>

#include "core/check.h"

namespace base {

// lower value means it has high priority
// evaluate unary operations first, then binary operations
// see README.md for detail.
enum class OperatorPrecedence : uint8_t {
  kUnknown = 0,
  kPostUnary = 1,                   // a++ a--
  kPreUnary = 2,                    // ++a --a !a ^a ~a
  kExponentiation = 3,              // a ** b
  kMultiplicative = 4,              // a * b a / b a % b
  kAdditive = 5,                    // a + b a - b
  kBitwiseShift = 6,                // a << b a >> b
  kThreeWayComparison = 7,          // a <=> b
  kRelationalComparison = 8,        // a < b a <= b a > b a >= b
  kEqualityComparison = 9,          // a == b a != b
  kBitwiseAnd = 10,                 // a & b
  kBitwiseXor = 11,                 // a ^ b
  kBitwiseOr = 12,                  // a \| b
  kLogicalAnd = 13,                 // a && b
  kLogicalOr = 14,                  // a \|\| b
  kAssignment = 15,                 // a = b a := b
  kCompoundAssignment = 15,         // a += b a -= b a *= b a /= b a %= b
  kBitwiseCompoundAssignment = 15,  // a &= b a ^= b a \|= b a <<= b a >>= b

  kInvalid = std::numeric_limits<uint8_t>::max(),
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
