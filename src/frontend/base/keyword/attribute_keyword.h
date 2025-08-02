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
  kUnsafe = 1,      // unsafe
  kFast = 2,        // fast
  kDefault = 3,     // default
  kDeprecated = 4,  // deprecated
  kPublic = 5,      // pub
};

inline AttributeKeyword token_kind_to_attribute_keyword(TokenKind kind) {
  switch (kind) {
    case TokenKind::kUnsafe: return AttributeKeyword::kUnsafe;
    case TokenKind::kFast: return AttributeKeyword::kFast;
    case TokenKind::kDefault: return AttributeKeyword::kDefault;
    case TokenKind::kDeprecated: return AttributeKeyword::kDeprecated;
    case TokenKind::kPublic: return AttributeKeyword::kPublic;
    default: return AttributeKeyword::kUnknown;
  }
}

inline bool token_kind_is_attribute_keyword(TokenKind kind) {
  return token_kind_to_attribute_keyword(kind) != AttributeKeyword::kUnknown;
}

inline const char* attribute_keyword_to_string(AttributeKeyword keyword) {
  switch (keyword) {
    case AttributeKeyword::kUnknown: return "unknown";
    case AttributeKeyword::kFast: return "fast";
    case AttributeKeyword::kDefault: return "default";
    case AttributeKeyword::kDeprecated: return "deprecated";
    case AttributeKeyword::kPublic: return "public";
    default: return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_ATTRIBUTE_KEYWORD_H_
