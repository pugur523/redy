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
  kPostIncrement = 1,               // a++ a--
  kPreIncrement = 2,                // ++a --a
  kLogicalNot = 2,                  // !a ~a
  kUnaryPlusMinus = 2,              // +a -a
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
    case OperatorPrecedence::kPostIncrement:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kPreIncrement:  // contains `kLogicalNot`,
                                             // `kUnaryPlusMinus`
      return OperatorAssociativity::kRightToLeft;
    case OperatorPrecedence::kExponentiation:
      return OperatorAssociativity::kRightToLeft;
    case OperatorPrecedence::kMultiplicative:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kAdditive:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kBitwiseShift:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kThreeWayComparison:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kRelationalComparison:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kEqualityComparison:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kBitwiseAnd:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kBitwiseXor:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kBitwiseOr:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kLogicalAnd:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kLogicalOr:
      return OperatorAssociativity::kLeftToRight;
    case OperatorPrecedence::kAssignment:  // contains `kCompoundAssignment`,
                                           // `kBitwiseCompoundAssignment`
      return OperatorAssociativity::kRightToLeft;
    default: DCHECK(false); return OperatorAssociativity::kUnknown;
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_OPERATOR_H_
