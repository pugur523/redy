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
  return TokenKind::kLiteralsBegin <= kind && kind <= TokenKind::kLiteralsEnd;
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

enum class LiteralType : uint8_t {
  kUnknown = 0,
  kI8 = 1,
  kI16 = 2,
  kI32 = 3,
  kI64 = 4,
  kI128 = 5,
  kU8 = 6,
  kU16 = 7,
  kU32 = 8,
  kU64 = 9,
  kU128 = 10,
  kF32 = 11,
  kF64 = 12,
  kCharacter = 13,
  kBool = 14,
  kString = 15,
};

}  // namespace base

#endif  // FRONTEND_BASE_LITERAL_LITERAL_H_
