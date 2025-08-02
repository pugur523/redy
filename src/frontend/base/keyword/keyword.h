// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_KEYWORD_H_
#define FRONTEND_BASE_KEYWORD_KEYWORD_H_

#include <string>

#include "frontend/base/base_export.h"
#include "frontend/base/token/token_kind.h"

namespace base {

inline bool is_keyword(TokenKind kind) {
  return kind >= TokenKind::kKeywordsBegin && kind <= TokenKind::kKeywordsEnd;
}

inline bool is_literal(TokenKind kind) {
  return kind >= TokenKind::kLiteralsBegin && kind <= TokenKind::kLiteralsEnd;
}

BASE_EXPORT TokenKind lookup_id_or_keyword(std::string_view word);

BASE_EXPORT TokenKind lookup_id_or_keyword(const std::string& full_source,
                                           std::size_t start,
                                           std::size_t length);

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_KEYWORD_H_
