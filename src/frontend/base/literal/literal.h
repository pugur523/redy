// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_LITERAL_LITERAL_H_
#define FRONTEND_BASE_LITERAL_LITERAL_H_

#include <cstdint>

#include "frontend/base/token/token_kind.h"

namespace base {

enum class LiteralKind : uint8_t {
  kUnknown = 0,

  // numeric
  kDecimal = 1,
  kBinary = 2,
  kOctal = 3,
  kHexadecimal = 4,

  kString = 5,
  kCharacter = 6,
  kTrue = 7,
  kFalse = 8
};

inline bool token_kind_is_literal(TokenKind kind) {
  return kind >= TokenKind::kLiteralsBegin && kind <= TokenKind::kLiteralsEnd;
}

inline LiteralKind token_kind_to_literal(base::TokenKind kind) {
  switch (kind) {
    case TokenKind::kDecimal: return LiteralKind::kDecimal;
    case TokenKind::kBinary: return LiteralKind::kBinary;
    case TokenKind::kOctal: return LiteralKind::kOctal;
    case TokenKind::kHexadecimal: return LiteralKind::kHexadecimal;
    case TokenKind::kString: return LiteralKind::kString;
    case TokenKind::kCharacter: return LiteralKind::kCharacter;
    case TokenKind::kTrue: return LiteralKind::kTrue;
    case TokenKind::kFalse: return LiteralKind::kFalse;

    default: return LiteralKind::kUnknown;
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_LITERAL_LITERAL_H_
