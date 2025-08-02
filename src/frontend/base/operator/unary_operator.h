// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_UNARY_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_UNARY_OPERATOR_H_

#include <cstdint>

#include "frontend/base/base_export.h"
#include "frontend/base/token/token.h"
#include "frontend/base/token/token_kind.h"

namespace base {

enum class UnaryOperator : uint8_t {
  kUnknown = 0,
  kNot = 1,         // ! (logical NOT)
  kNegate = 2,      // - (unary minus)
  kBitwiseNot = 3,  // ~ (bitwise NOT)
  kIncrement = 4,   // ++
  kDecrement = 5,   // --
};

inline UnaryOperator token_kind_to_unary_op(TokenKind kind) {
  switch (kind) {
    case TokenKind::kBang: return UnaryOperator::kNot;              // !
    case TokenKind::kMinus: return UnaryOperator::kNegate;          // -
    case TokenKind::kTilde: return UnaryOperator::kBitwiseNot;      // ~
    case TokenKind::kPlusPlus: return UnaryOperator::kIncrement;    // ++
    case TokenKind::kMinusMinus: return UnaryOperator::kDecrement;  // --
    default: return UnaryOperator::kUnknown;
  }
}

inline bool token_kind_is_unary_operator(TokenKind kind) {
  return token_kind_to_unary_op(kind) != UnaryOperator::kUnknown;
}

inline const char* unary_op_to_string(UnaryOperator op) {
  switch (op) {
    case UnaryOperator::kUnknown: return "unknown";
    case UnaryOperator::kNot: return "not";
    case UnaryOperator::kNegate: return "negate";
    case UnaryOperator::kBitwiseNot: return "bitwise not";
    case UnaryOperator::kIncrement: return "increment";
    case UnaryOperator::kDecrement: return "decrement";
    default: DCHECK(false); return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_UNARY_OPERATOR_H_
