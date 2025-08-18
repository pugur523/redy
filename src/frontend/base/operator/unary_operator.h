// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_OPERATOR_UNARY_OPERATOR_H_
#define FRONTEND_BASE_OPERATOR_UNARY_OPERATOR_H_

#include <cstdint>

#include "frontend/base/operator/operator.h"
#include "frontend/base/token/token_kind.h"
#include "i18n/base/data/translation_key.h"

namespace base {

enum class UnaryOperator : uint8_t {
  kUnknown = 0,
  kPostfixIncrement = 1,  // a++
  kPostfixDecrement = 2,  // a--
  kPrefixIncrement = 3,   // ++a
  kPrefixDecrement = 4,   // --a
  kNot = 5,               // !a
  kBitwiseNot = 6,        // ~a
  kUnaryPlus = 7,         // +a
  kUnaryMinus = 8,        // -a
};

enum class IncrementPosition : uint8_t {
  kUnknown = 0,
  kPostfix = 1,
  kPrefix = 2,
};

// designate pos as `IncrementPosition::kPostfix` or `kPrefix` to distinguish
// post/pre increment(decrement).
// no need to specify it when call with a token like `!`, `~`, `+`, `-`
inline UnaryOperator token_kind_to_unary_op(
    TokenKind kind,
    IncrementPosition pos = IncrementPosition::kUnknown) {
  switch (kind) {
    case TokenKind::kPlusPlus:  // ++
      switch (pos) {
        case IncrementPosition::kPostfix:
          return UnaryOperator::kPostfixIncrement;
        case IncrementPosition::kPrefix: return UnaryOperator::kPrefixIncrement;
        default: DCHECK(false); return UnaryOperator::kUnknown;
      }
    case TokenKind::kMinusMinus:  // --
      switch (pos) {
        case IncrementPosition::kPostfix:
          return UnaryOperator::kPostfixDecrement;
        case IncrementPosition::kPrefix: return UnaryOperator::kPrefixDecrement;
        default: DCHECK(false); return UnaryOperator::kUnknown;
      }
    case TokenKind::kBang: return UnaryOperator::kNot;          // !
    case TokenKind::kTilde: return UnaryOperator::kBitwiseNot;  // ~
    case TokenKind::kPlus: return UnaryOperator::kUnaryPlus;    // +
    case TokenKind::kMinus: return UnaryOperator::kUnaryMinus;  // -
    default: return UnaryOperator::kUnknown;
  }
}

inline bool token_kind_is_unary_operator(TokenKind kind) {
  switch (kind) {
    case TokenKind::kPlusPlus:
    case TokenKind::kMinusMinus:
    case TokenKind::kBang:
    case TokenKind::kTilde:
    case TokenKind::kPlus:
    case TokenKind::kMinus: return true;
    default: return false;
  }
}

inline const char* unary_op_to_string(UnaryOperator op) {
  static constexpr const char* kNames[] = {
      "unknown",          "postfix increment", "postfix decrement",
      "prefix increment", "prefix decrement",  "not",
      "bitwise not",      "unary plus",        "unary minus",
      "invalid",
  };
  const auto idx = static_cast<std::size_t>(op);
  DCHECK_LT(idx, std::size(kNames));
  return kNames[idx];
}

inline OperatorPrecedence unary_op_to_precedence(UnaryOperator op) {
  switch (op) {
    case UnaryOperator::kPostfixIncrement:
    case UnaryOperator::kPostfixDecrement:
      return OperatorPrecedence::kPostUnary;
    default: return OperatorPrecedence::kPreUnary;
  }
}

inline bool is_postfix_operator(UnaryOperator op) {
  return op != UnaryOperator::kPrefixIncrement &&
         op != UnaryOperator::kPrefixDecrement && op != UnaryOperator::kUnknown;
}

}  // namespace base

#endif  // FRONTEND_BASE_OPERATOR_UNARY_OPERATOR_H_
