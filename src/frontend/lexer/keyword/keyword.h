// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_KEYWORD_KEYWORD_H_
#define FRONTEND_LEXER_KEYWORD_KEYWORD_H_

#include <string>

#include "frontend/lexer/base/lexer_export.h"

namespace lexer {

enum class TokenKind : uint8_t;

LEXER_EXPORT TokenKind
lookup_identifier_or_keyword(const std::string_view& word);

LEXER_EXPORT TokenKind lookup_identifier_or_keyword(const std::string& source,
                                                    std::size_t start,
                                                    std::size_t length);

}  // namespace lexer

#endif  // FRONTEND_LEXER_KEYWORD_KEYWORD_H_
