// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_ATTRIBUTE_KEYWORD_H_
#define FRONTEND_BASE_KEYWORD_ATTRIBUTE_KEYWORD_H_

#include <cstdint>

#include "frontend/base/token/token_kind.h"

namespace base {

enum class AttributeKeyword : uint8_t {
  kUnknown = 0,
  kMutable = 1,      // mut
  kConstant = 2,     // const
  kExtern = 3,       // extern
  kStatic = 4,       // static
  kThreadLocal = 5,  // thread_local
  kPublic = 6,       // pub
  kAsync = 7,        // async
};

inline AttributeKeyword token_kind_to_attribute_keyword(TokenKind kind) {
  switch (kind) {
    case TokenKind::kMutable: return AttributeKeyword::kMutable;
    case TokenKind::kConstant: return AttributeKeyword::kConstant;
    case TokenKind::kExtern: return AttributeKeyword::kExtern;
    case TokenKind::kStatic: return AttributeKeyword::kStatic;
    case TokenKind::kThreadLocal: return AttributeKeyword::kThreadLocal;
    case TokenKind::kPublic: return AttributeKeyword::kPublic;
    case TokenKind::kAsync: return AttributeKeyword::kAsync;
    default: return AttributeKeyword::kUnknown;
  }
}

inline bool token_kind_is_attribute_keyword(TokenKind kind) {
  return token_kind_to_attribute_keyword(kind) != AttributeKeyword::kUnknown;
}

inline const char* attribute_keyword_to_string(AttributeKeyword keyword) {
  switch (keyword) {
    case AttributeKeyword::kUnknown: return "unknown";
    case AttributeKeyword::kMutable: return "mutable";
    case AttributeKeyword::kConstant: return "constant";
    case AttributeKeyword::kExtern: return "extern";
    case AttributeKeyword::kStatic: return "static";
    case AttributeKeyword::kThreadLocal: return "thread local";
    case AttributeKeyword::kPublic: return "public";
    case AttributeKeyword::kAsync: return "async";
    default: return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_ATTRIBUTE_KEYWORD_H_
