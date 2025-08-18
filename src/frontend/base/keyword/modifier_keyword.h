// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_MODIFIER_KEYWORD_H_
#define FRONTEND_BASE_KEYWORD_MODIFIER_KEYWORD_H_

#include <cstdint>

#include "frontend/base/token/token_kind.h"

namespace base {

enum class ModifierKeyword : uint8_t {
  kUnknown = 0,
  kThis = 1,   // this
  kAs = 2,     // as
  kAwait = 3,  // await
};

inline ModifierKeyword token_kind_to_modifier_keyword(TokenKind kind) {
  switch (kind) {
    case TokenKind::kThis: return ModifierKeyword::kThis;
    case TokenKind::kAs: return ModifierKeyword::kAs;
    case TokenKind::kAwait: return ModifierKeyword::kAwait;
    default: return ModifierKeyword::kUnknown;
  }
}

inline bool token_kind_is_modifier_keyword(TokenKind kind) {
  return token_kind_to_modifier_keyword(kind) != ModifierKeyword::kUnknown;
}

inline const char* modifier_keyword_to_string(ModifierKeyword keyword) {
  switch (keyword) {
    case ModifierKeyword::kThis: return "this";
    case ModifierKeyword::kAs: return "as";
    case ModifierKeyword::kAwait: return "await";
    default: return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_MODIFIER_KEYWORD_H_
